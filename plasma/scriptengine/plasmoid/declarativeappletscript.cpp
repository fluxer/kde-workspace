/*
 *   Copyright 2009 by Alan Alpert <alan.alpert@nokia.com>
 *   Copyright 2010 by Ménard Alexis <menard@kde.org>

 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeExpression>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QTimer>
#include <QUiLoader>
#include <QWidget>

#include <KConfigGroup>
#include <KDebug>
#include <KGlobalSettings>

#include <Plasma/Applet>
#include <Plasma/DeclarativeWidget>
#include <Plasma/Extender>
#include <Plasma/ExtenderItem>
#include <Plasma/FrameSvg>
#include <Plasma/Package>
#include <Plasma/PopupApplet>
#include <Plasma/Svg>


#include "plasmoid/declarativeappletscript.h"

#include "plasmoid/appletinterface.h"
#include "plasmoid/themedsvg.h"

#include "common/scriptenv.h"
#include "declarative/declarativeitemcontainer_p.h"
#include "simplebindings/bytearrayclass.h"
#include "simplebindings/dataengine.h"
#include "simplebindings/dataenginereceiver.h"


K_EXPORT_PLASMA_APPLETSCRIPTENGINE(declarativeappletscript, DeclarativeAppletScript)


QScriptValue constructIconClass(QScriptEngine *engine);
QScriptValue constructKUrlClass(QScriptEngine *engine);
QScriptValue constructQPointClass(QScriptEngine *engine);
void registerSimpleAppletMetaTypes(QScriptEngine *engine);
DeclarativeAppletScript::DeclarativeAppletScript(QObject *parent, const QVariantList &args)
    : AbstractJsAppletScript(parent, args),
      m_declarativeWidget(0),
      m_toolBoxWidget(0),
      m_interface(0),
      m_engine(0),
      m_env(0)
{
    Q_UNUSED(args);
}

DeclarativeAppletScript::~DeclarativeAppletScript()
{
}

bool DeclarativeAppletScript::init()
{
    m_declarativeWidget = new Plasma::DeclarativeWidget(applet());
    m_declarativeWidget->setInitializationDelayed(true);
    connect(m_declarativeWidget, SIGNAL(finished()), this, SLOT(qmlCreationFinished()));
    KGlobal::locale()->insertCatalog("plasma_applet_" + description().pluginName());

    //make possible to import extensions from the package
    //FIXME: probably to be removed, would make possible to use native code from within the package :/
    //m_declarativeWidget->engine()->addImportPath(package()->path()+"/contents/imports");
    m_declarativeWidget->setQmlPath(mainScript());

    if (!m_declarativeWidget->engine() || !m_declarativeWidget->engine()->rootContext() || !m_declarativeWidget->engine()->rootContext()->isValid() || m_declarativeWidget->mainComponent()->isError()) {
        QString reason;
        foreach (QDeclarativeError error, m_declarativeWidget->mainComponent()->errors()) {
            reason += error.toString()+'\n';
        }
        setFailedToLaunch(true, reason);
        return false;
    }

    Plasma::Applet *a = applet();
    Plasma::PopupApplet *pa = qobject_cast<Plasma::PopupApplet *>(a);
    Plasma::Containment *cont = qobject_cast<Plasma::Containment *>(a);

    if (pa) {
        pa->setPopupIcon(a->icon());
        pa->setGraphicsWidget(m_declarativeWidget);
    } else {
        QGraphicsLinearLayout *lay = new QGraphicsLinearLayout(a);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->addItem(m_declarativeWidget);
    }

    if (pa) {
        m_interface = new PopupAppletInterface(this);
    } else if (cont) {
        m_interface = new ContainmentInterface(this);
    //fail? so it's a normal Applet
    } else {
        m_interface = new AppletInterface(this);
    }

    connect(applet(), SIGNAL(extenderItemRestored(Plasma::ExtenderItem*)),
            this, SLOT(extenderItemRestored(Plasma::ExtenderItem*)));
    connect(applet(), SIGNAL(activate()),
            this, SLOT(activate()));

    setupObjects();

    return true;
}

void DeclarativeAppletScript::qmlCreationFinished()
{
    //If it's a popupapplet and the root object has a "compactRepresentation" component, use that instead of the icon
    Plasma::Applet *a = applet();
    Plasma::PopupApplet *pa = qobject_cast<Plasma::PopupApplet *>(a);
    m_self.setProperty("rootItem", m_engine->newQObject(m_declarativeWidget->rootObject()));

    if (pa) {
        QDeclarativeComponent *iconComponent = m_declarativeWidget->rootObject()->property("compactRepresentation").value<QDeclarativeComponent *>();
        if (iconComponent) {
            QDeclarativeItem *declarativeIcon = qobject_cast<QDeclarativeItem *>(iconComponent->create(iconComponent->creationContext()));
            if (declarativeIcon) {
                pa->setPopupIcon(QIcon());
                QGraphicsLinearLayout *lay = new QGraphicsLinearLayout(a);
                lay->setContentsMargins(0, 0, 0, 0);
                DeclarativeItemContainer *declarativeItemContainer = new DeclarativeItemContainer(a);
                lay->addItem(declarativeItemContainer);
                declarativeItemContainer->setDeclarativeItem(declarativeIcon, true);
            } else {
                pa->setPopupIcon(a->icon());
            }
        } else {
            pa->setPopupIcon(a->icon());
        }
    }

    Plasma::Containment *pc = qobject_cast<Plasma::Containment *>(a);
    if (pc) {
        Plasma::PackageStructure::Ptr structure = Plasma::PackageStructure::load("Plasma/Generic");
        Plasma::Package pkg = Plasma::Package(QString(), "org.kde.toolbox", structure);
        if (pkg.isValid()) {
            const QString qmlPath = pkg.filePath("mainscript");

            m_toolBoxWidget = new Plasma::DeclarativeWidget(pc);
            m_toolBoxWidget->setInitializationDelayed(true);
            m_toolBoxWidget->setQmlPath(qmlPath);

            QGraphicsLinearLayout *toolBoxScreenLayout = new QGraphicsLinearLayout(m_declarativeWidget);
            toolBoxScreenLayout->addItem(m_toolBoxWidget);
            toolBoxScreenLayout->setContentsMargins(0, 0, 0, 0);

            QScriptEngine *engine = m_toolBoxWidget->scriptEngine();
            if (!engine) {
                return;
            }
            QScriptValue global = engine->globalObject();
            m_self = engine->newQObject(m_interface);
            global.setProperty("plasmoid", m_self);
        } else {
            kWarning() << "Could not load org.kde.toolbox package.";
        }
    }
}

void DeclarativeAppletScript::collectGarbage()
{
    if (!m_engine) {
        return;
    }
    m_engine->collectGarbage();
}

QString DeclarativeAppletScript::filePath(const QString &type, const QString &file) const
{
    const QString path = m_env->filePathFromScriptContext(type.toLocal8Bit().constData(), file);

    if (!path.isEmpty()) {
        return path;
    }

    return package()->filePath(type.toLocal8Bit().constData(), file);
}

void DeclarativeAppletScript::configChanged()
{
    if (!m_env) {
        return;
    }

    m_env->callEventListeners("configchanged");
}

QScriptValue DeclarativeAppletScript::loadui(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1) {
        return context->throwError(i18n("loadui() takes one argument"));
    }

    QString filename = context->argument(0).toString();
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return context->throwError(i18n("Unable to open '%1'",filename));
    }

    QUiLoader loader;
    QWidget *w = loader.load(&f);
    f.close();

    return engine->newQObject(w, QScriptEngine::AutoOwnership);
}

QScriptValue DeclarativeAppletScript::newPlasmaSvg(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() == 0) {
        return context->throwError(i18n("Constructor takes at least 1 argument"));
    }

    const QString filename = context->argument(0).toString();
    Plasma::Svg *svg = new ThemedSvg(0);
    svg->setImagePath(ThemedSvg::findSvg(engine, filename));

    QScriptValue obj = engine->newQObject(svg);
    ScriptEnv::registerEnums(obj, *svg->metaObject());

    return obj;
}

QScriptValue DeclarativeAppletScript::variantToScriptValue(QVariant var)
{
    if (!m_engine) {
        return QScriptValue();
    }
    return m_engine->newVariant(var);
}

QScriptValue DeclarativeAppletScript::newPlasmaFrameSvg(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() == 0) {
        return context->throwError(i18n("Constructor takes at least 1 argument"));
    }

    QString filename = context->argument(0).toString();

    bool parentedToApplet = false;
    QGraphicsWidget *parent = extractParent(context, engine, 1, &parentedToApplet);
    Plasma::FrameSvg *frameSvg = new ThemedFrameSvg(parent);
    frameSvg->setImagePath(ThemedSvg::findSvg(engine, filename));

    QScriptValue obj = engine->newQObject(frameSvg);
    ScriptEnv::registerEnums(obj, *frameSvg->metaObject());

    return obj;
}

QScriptValue DeclarativeAppletScript::newPlasmaExtenderItem(QScriptContext *context, QScriptEngine *engine)
{
    Plasma::Extender *extender = 0;
    if (context->argumentCount() > 0) {
        extender = qobject_cast<Plasma::Extender *>(context->argument(0).toQObject());
    }

    if (!extender) {
        AppletInterface *interface = AppletInterface::extract(engine);
        if (!interface) {
            return engine->undefinedValue();
        }

        extender = interface->extender();
    }

    Plasma::ExtenderItem *extenderItem = new Plasma::ExtenderItem(extender);
    QScriptValue fun = engine->newQObject(extenderItem);
    ScriptEnv::registerEnums(fun, *extenderItem->metaObject());
    return fun;
}

QGraphicsWidget *DeclarativeAppletScript::extractParent(QScriptContext *context, QScriptEngine *engine,
                                                       int argIndex, bool *parentedToApplet)
{
    if (parentedToApplet) {
        *parentedToApplet = false;
    }

    QGraphicsWidget *parent = 0;
    if (context->argumentCount() >= argIndex) {
        parent = qobject_cast<QGraphicsWidget*>(context->argument(argIndex).toQObject());
    }

    if (!parent) {
        AppletInterface *interface = AppletInterface::extract(engine);
        if (!interface) {
            return 0;
        }

        //kDebug() << "got the applet!";
        parent = interface->applet();

        if (parentedToApplet) {
            *parentedToApplet = true;
        }
    }

    return parent;
}

void DeclarativeAppletScript::callPlasmoidFunction(const QString &functionName, const QScriptValueList &args, ScriptEnv *env)
{
    if (!m_env) {
        m_env = ScriptEnv::findScriptEnv(m_engine);
    }

    if (env) {
        QScriptValue func = m_self.property(functionName);
        m_env->callFunction(func, args, m_self);
    }
}

void DeclarativeAppletScript::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint) {
        emit formFactorChanged();
    }

    if (constraints & Plasma::LocationConstraint) {
        emit locationChanged();
    }

    if (constraints & Plasma::ContextConstraint) {
        emit contextChanged();
    }
}

void DeclarativeAppletScript::popupEvent(bool popped)
{
    if (!m_env) {
        return;
    }

    QScriptValueList args;
    args << popped;

    m_env->callEventListeners("popupEvent", args);
}

void DeclarativeAppletScript::dataUpdated(const QString &name, const Plasma::DataEngine::Data &data)
{
    if (!m_engine) {
        return;
    }
    QScriptValueList args;
    args << m_engine->toScriptValue(name) << m_engine->toScriptValue(data);

    m_env->callEventListeners("dataUpdated", args);
}

void DeclarativeAppletScript::extenderItemRestored(Plasma::ExtenderItem* item)
{
    if (!m_env) {
        return;
    }
    if (!m_engine) {
        return;
    }

    QScriptValueList args;
    args << m_engine->newQObject(item, QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);

    m_env->callEventListeners("initExtenderItem", args);
}

void DeclarativeAppletScript::activate()
{
    if (!m_env) {
        return;
    }

    m_env->callEventListeners("activate");
}

void DeclarativeAppletScript::executeAction(const QString &name)
{
    if (!m_env) {
        return;
    }

    if (m_declarativeWidget->rootObject()) {
         QMetaObject::invokeMethod(m_declarativeWidget->rootObject(), QString("action_" + name).toLatin1(), Qt::DirectConnection);
    }
}

bool DeclarativeAppletScript::include(const QString &path)
{
    return m_env->include(path);
}

ScriptEnv *DeclarativeAppletScript::scriptEnv()
{
    return m_env;
}

void DeclarativeAppletScript::setupObjects()
{
    m_engine = m_declarativeWidget->scriptEngine();
    if (!m_engine) {
        return;
    }

    connect(m_engine, SIGNAL(signalHandlerException(const QScriptValue &)),
            this, SLOT(signalHandlerException(const QScriptValue &)));

    delete m_env;
    m_env = new ScriptEnv(this, m_engine);

    QScriptValue global = m_engine->globalObject();

    QScriptValue v = m_engine->newVariant(QVariant::fromValue(*applet()->package()));
    global.setProperty("__plasma_package", v,
                       QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);

    m_self = m_engine->newQObject(m_interface);
    global.setProperty("plasmoid", m_self);
    m_env->addMainObjectProperties(m_self);

    QScriptValue args = m_engine->newArray();
    int i = 0;
    foreach (const QVariant &arg, applet()->startupArguments()) {
        args.setProperty(i, m_engine->newVariant(arg));
        ++i;
    }
    global.setProperty("startupArguments", args);

    // Add a global loadui method for ui files
    QScriptValue fun = m_engine->newFunction(DeclarativeAppletScript::loadui);
    global.setProperty("loadui", fun);

    ScriptEnv::registerEnums(global, AppletInterface::staticMetaObject);
    //Make enum values accessible also as plasmoid.Planar etc
    ScriptEnv::registerEnums(m_self, AppletInterface::staticMetaObject);

    global.setProperty("dataEngine", m_engine->newFunction(DeclarativeAppletScript::dataEngine));
    global.setProperty("service", m_engine->newFunction(DeclarativeAppletScript::service));

    //Add stuff from Qt
    //TODO: move to libkdeclarative?
    ByteArrayClass *baClass = new ByteArrayClass(m_engine);
    global.setProperty("ByteArray", baClass->constructor());
    global.setProperty("QPoint", constructQPointClass(m_engine));

    // Add stuff from KDE libs
    qScriptRegisterSequenceMetaType<KUrl::List>(m_engine);
    global.setProperty("Url", constructKUrlClass(m_engine));

    // Add stuff from Plasma
    global.setProperty("Svg", m_engine->newFunction(DeclarativeAppletScript::newPlasmaSvg));
    global.setProperty("FrameSvg", m_engine->newFunction(DeclarativeAppletScript::newPlasmaFrameSvg));
    global.setProperty("ExtenderItem", m_engine->newFunction(DeclarativeAppletScript::newPlasmaExtenderItem));

    registerSimpleAppletMetaTypes(m_engine);
    QTimer::singleShot(0, this, SLOT(configChanged()));
}

QScriptValue DeclarativeAppletScript::dataEngine(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1) {
        return context->throwError(i18n("dataEngine() takes one argument"));
    }

    AppletInterface *interface = AppletInterface::extract(engine);
    if (!interface) {
        return context->throwError(i18n("Could not extract the Applet"));
    }

    const QString dataEngineName = context->argument(0).toString();
    Plasma::DataEngine *dataEngine = interface->dataEngine(dataEngineName);
    QScriptValue v = engine->newQObject(dataEngine, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
    v.setProperty("connectSource", engine->newFunction(DataEngineReceiver::connectSource));
    v.setProperty("connectAllSources", engine->newFunction(DataEngineReceiver::connectAllSources));
    v.setProperty("disconnectSource", engine->newFunction(DataEngineReceiver::disconnectSource));
    return v;
}

QScriptValue DeclarativeAppletScript::service(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 2) {
        return context->throwError(i18n("service() takes two arguments"));
    }

    QString dataEngine = context->argument(0).toString();

    AppletInterface *interface = AppletInterface::extract(engine);
    if (!interface) {
        return context->throwError(i18n("Could not extract the Applet"));
    }

    Plasma::DataEngine *data = interface->dataEngine(dataEngine);
    QString source = context->argument(1).toString();
    Plasma::Service *service = data->serviceForSource(source);
    //kDebug( )<< "lets try to get" << source << "from" << dataEngine;
    return engine->newQObject(service, QScriptEngine::AutoOwnership);
}

QList<QAction*> DeclarativeAppletScript::contextualActions()
{
    if (!m_interface) {
        return QList<QAction *>();
    }

    return m_interface->contextualActions();
}

QScriptEngine *DeclarativeAppletScript::engine() const
{
    return m_engine;
}

void DeclarativeAppletScript::signalHandlerException(const QScriptValue &exception)
{
    kWarning()<<"Exception caught: "<<exception.toVariant();
}

#include "moc_declarativeappletscript.cpp"

