/*
 *   Copyright 2009 Aaron Seigo <aseigo@kde.org>
 *
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

#include "scriptengine.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QScriptValueIterator>

#include <KDebug>
#include <kdeversion.h>
#include <KGlobalSettings>
#include <KMimeTypeTrader>
#include <KServiceTypeTrader>
#include <KShell>
#include <KStandardDirs>

#include <Plasma/Applet>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/Package>
#include <Plasma/Wallpaper>

#include "appinterface.h"
#include "containment.h"
#include "configgroup.h"
#include "i18n.h"
#include "layouttemplatepackagestructure.h"
#include "widget.h"

QScriptValue constructQRectFClass(QScriptEngine *engine);

namespace WorkspaceScripting
{

ScriptEngine::ScriptEngine(Plasma::Corona *corona, QObject *parent)
    : QScriptEngine(parent),
      m_corona(corona)
{
    Q_ASSERT(m_corona);
    AppInterface *interface = new AppInterface(this);
    connect(interface, SIGNAL(print(QString)), this, SIGNAL(print(QString)));
    m_scriptSelf = newQObject(interface, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeSuperClassProperties |
                              QScriptEngine::ExcludeSuperClassMethods);
    setupEngine();
    connect(this, SIGNAL(signalHandlerException(QScriptValue)), this, SLOT(exception(QScriptValue)));
    bindI18N(this);
}

ScriptEngine::~ScriptEngine()
{
}

QScriptValue ScriptEngine::newActivity(QScriptContext *context, QScriptEngine *engine)
{
   return createContainment("desktop", "desktop", context, engine);
}

QScriptValue ScriptEngine::newPanel(QScriptContext *context, QScriptEngine *engine)
{
    return createContainment("panel", "panel", context, engine);
}

QScriptValue ScriptEngine::createContainment(const QString &type, const QString &defaultPlugin,
                                             QScriptContext *context, QScriptEngine *engine)
{
    QString plugin = context->argumentCount() > 0 ? context->argument(0).toString() :
                                                    defaultPlugin;

    bool exists = false;
    const KPluginInfo::List list = Plasma::Containment::listContainmentsOfType(type);
    foreach (const KPluginInfo &info, list) {
        if (info.pluginName() == plugin) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        return context->throwError(i18n("Could not find a plugin for %1 named %2.", type, plugin));
    }


    ScriptEngine *env = envFor(engine);
    Plasma::Containment *c = env->m_corona->addContainment(plugin);
    if (c) {
        if (type == "panel") {
            // some defaults
            c->setScreen(env->defaultPanelScreen());
            c->setLocation(Plasma::TopEdge);
        }
        c->updateConstraints(Plasma::AllConstraints | Plasma::StartupCompletedConstraint);
        c->flushPendingConstraintsEvents();
        emit env->createPendingPanelViews();
    }

    return env->wrap(c);
}

QScriptValue ScriptEngine::wrap(Plasma::Applet *w)
{
    Widget *wrapper = new Widget(w);
    QScriptValue v = newQObject(wrapper, QScriptEngine::ScriptOwnership,
                                QScriptEngine::ExcludeSuperClassProperties |
                                QScriptEngine::ExcludeSuperClassMethods);
    return v;
}

QScriptValue ScriptEngine::wrap(Plasma::Containment *c)
{
    Containment *wrapper = new Containment(c);
    return wrap(wrapper);
}

QScriptValue ScriptEngine::wrap(Containment *c)
{
    QScriptValue v = newQObject(c, QScriptEngine::ScriptOwnership,
                                QScriptEngine::ExcludeSuperClassProperties |
                                QScriptEngine::ExcludeSuperClassMethods);
    v.setProperty("widgetById", newFunction(Containment::widgetById));
    v.setProperty("addWidget", newFunction(Containment::addWidget));
    v.setProperty("widgets", newFunction(Containment::widgets));

    return v;
}

int ScriptEngine::defaultPanelScreen() const
{
    return qApp ? qApp->desktop()->primaryScreen() : 0;
}

ScriptEngine *ScriptEngine::envFor(QScriptEngine *engine)
{
    QObject *object = engine->globalObject().toQObject();
    Q_ASSERT(object);

    AppInterface *interface = qobject_cast<AppInterface *>(object);
    Q_ASSERT(interface);

    ScriptEngine *env = qobject_cast<ScriptEngine *>(interface->parent());
    Q_ASSERT(env);

    return env;
}

QScriptValue ScriptEngine::panelById(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() == 0) {
        return context->throwError(i18n("panelById requires an id"));
    }

    const uint id = context->argument(0).toInt32();
    ScriptEngine *env = envFor(engine);
    foreach (Plasma::Containment *c, env->m_corona->containments()) {
        if (c->id() == id && isPanel(c)) {
            return env->wrap(c);
        }
    }

    return engine->undefinedValue();
}

QScriptValue ScriptEngine::panels(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(context)

    QScriptValue panels = engine->newArray();
    ScriptEngine *env = envFor(engine);
    int count = 0;

    foreach (Plasma::Containment *c, env->m_corona->containments()) {
        if (isPanel(c)) {
            panels.setProperty(count, env->wrap(c));
            ++count;
        }
    }

    panels.setProperty("length", count);
    return panels;
}

QScriptValue ScriptEngine::fileExists(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    const QString path = context->argument(0).toString();
    if (path.isEmpty()) {
        return false;
    }

    QFile f(KShell::tildeExpand(path));
    return f.exists();
}

QScriptValue ScriptEngine::loadTemplate(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        kDebug() << "no arguments";
        return false;
    }

    const QString layout = context->argument(0).toString();
    if (layout.isEmpty() || layout.contains("'")) {
        kDebug() << "layout is empty";
        return false;
    }

    const QString constraint = QString("[X-Plasma-Shell] == '%1' and [X-KDE-PluginInfo-Name] == '%2'")
                                      .arg(KGlobal::mainComponent().componentName(),layout);
    KService::List offers = KServiceTypeTrader::self()->query("Plasma/LayoutTemplate", constraint);

    if (offers.isEmpty()) {
        kDebug() << "offers fail" << constraint;
        return false;
    }

    Plasma::PackageStructure::Ptr structure(new LayoutTemplatePackageStructure);
    KPluginInfo info(offers.first());
    const QString path = KStandardDirs::locate("data", structure->defaultPackageRoot() + '/' + info.pluginName() + '/');
    if (path.isEmpty()) {
        kDebug() << "script path is empty";
        return false;
    }

    Plasma::Package package(path, structure);
    const QString scriptFile = package.filePath("mainscript");
    if (scriptFile.isEmpty()) {
        kDebug() << "scriptfile is empty";
        return false;
    }

    QFile file(scriptFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kWarning() << i18n("Unable to load script file: %1", path);
        return false;
    }

    QString script = file.readAll();
    if (script.isEmpty()) {
        kDebug() << "script is empty";
        return false;
    }

    ScriptEngine *env = envFor(engine);
    env->globalObject().setProperty("templateName", env->newVariant(info.name()), QScriptValue::ReadOnly | QScriptValue::Undeletable);
    env->globalObject().setProperty("templateComment", env->newVariant(info.comment()), QScriptValue::ReadOnly | QScriptValue::Undeletable);

    QScriptValue rv = env->newObject();
    QScriptContext *ctx = env->pushContext();
    ctx->setThisObject(rv);

    env->evaluateScript(script, path);

    env->popContext();
    return rv;
}

QScriptValue ScriptEngine::applicationExists(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    const QString application = context->argument(0).toString();
    if (application.isEmpty()) {
        return false;
    }

    // first, check for it in $PATH
    if (!KStandardDirs::findExe(application).isEmpty()) {
        return true;
    }

    if (KService::serviceByStorageId(application)) {
        return true;
    }

    if (application.contains("'")) {
        // apostrophes just screw up the trader lookups below, so check for it
        return false;
    }

    // next, consult ksycoca for an app by that name
    if (!KServiceTypeTrader::self()->query("Application", QString("Name =~ '%1'").arg(application)).isEmpty()) {
        return true;
    }

    // next, consult ksycoca for an app by that generic name
    if (!KServiceTypeTrader::self()->query("Application", QString("GenericName =~ '%1'").arg(application)).isEmpty()) {
        return true;
    }

    return false;
}

QString ScriptEngine::onlyExec(const QString &commandLine)
{
    if (commandLine.isEmpty()) {
        return commandLine;
    }

    return KShell::splitArgs(commandLine, KShell::TildeExpand).first();
}

QScriptValue ScriptEngine::applicationPath(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return false;
    }

    const QString application = context->argument(0).toString();
    if (application.isEmpty()) {
        return false;
    }

    // first, check for it in $PATH
    const QString path = KStandardDirs::findExe(application);
    if (!path.isEmpty()) {
        return path;
    }

    if (KService::Ptr service = KService::serviceByStorageId(application)) {
        return KStandardDirs::locate("apps", service->entryPath());
    }

    if (application.contains("'")) {
        // apostrophes just screw up the trader lookups below, so check for it
        return QString();
    }

    // next, consult ksycoca for an app by that name
    KService::List offers = KServiceTypeTrader::self()->query("Application", QString("Name =~ '%1'").arg(application));
    if (offers.isEmpty()) {
        // next, consult ksycoca for an app by that generic name
        offers = KServiceTypeTrader::self()->query("Application", QString("GenericName =~ '%1'").arg(application));
    }

    if (!offers.isEmpty()) {
        KService::Ptr offer = offers.first();
        return KStandardDirs::locate("apps", offer->entryPath());
    }

    return QString();
}

QScriptValue ScriptEngine::userDataPath(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    if (context->argumentCount() == 0) {
        return QDir::homePath();
    }

    const QString type = context->argument(0).toString();
    if (type.isEmpty()) {
        return QDir::homePath();
    }

    if (context->argumentCount() > 1) {
        const QString filename = context->argument(1).toString();
        return KStandardDirs::locateLocal(type.toLatin1(), filename);
    }

    if (type.compare("desktop", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::desktopPath();
    } else if (type.compare("documents", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::documentPath();
    } else if (type.compare("music", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::musicPath();
    } else if (type.compare("video", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::videosPath();
    } else if (type.compare("downloads", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::downloadPath();
    } else if (type.compare("pictures", Qt::CaseInsensitive) == 0) {
        return KGlobalSettings::picturesPath();
    }

    return QString();
}

QScriptValue ScriptEngine::knownWallpaperPlugins(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)

    QString formFactor;
    if (context->argumentCount() > 0) {
        formFactor = context->argument(0).toString();
    }

    QString constraint;
    if (!formFactor.isEmpty()) {
        constraint.append("[X-Plasma-FormFactors] ~~ '").append(formFactor).append("'");
    }

    KService::List services = KServiceTypeTrader::self()->query("Plasma/Wallpaper", constraint);
    QScriptValue rv = engine->newArray(services.size());
    foreach (const KService::Ptr service, services) {
        QList<KServiceAction> modeActions = service->actions();
        QScriptValue modes = engine->newArray(modeActions.size());
        int i = 0;
        foreach (const KServiceAction &action, modeActions) {
            modes.setProperty(i++, action.name());
        }

        rv.setProperty(service->name(), modes);
    }

    return rv;
}

QScriptValue ScriptEngine::configFile(QScriptContext *context, QScriptEngine *engine)
{
    ConfigGroup *file = 0;

    if (context->argumentCount() > 0) {
        if (context->argument(0).isString()) {
            file = new ConfigGroup;
            file->setFile(context->argument(0).toString());
            if (context->argumentCount() > 1) {
                file->setGroup(context->argument(1).toString());
            }
        } else if (ConfigGroup *parent= qobject_cast<ConfigGroup *>(context->argument(0).toQObject())) {
            file = new ConfigGroup(parent);
        }
    } else {
        file = new ConfigGroup;
    }

    QScriptValue v = engine->newQObject(file,
                                        QScriptEngine::ScriptOwnership,
                                        QScriptEngine::ExcludeSuperClassProperties |
                                        QScriptEngine::ExcludeSuperClassMethods);
    return v;

}

void ScriptEngine::setupEngine()
{
    QScriptValue v = globalObject();
    QScriptValueIterator it(v);
    while (it.hasNext()) {
        it.next();
        // we provide our own print implementation, but we want the rest
        if (it.name() != "print") {
            m_scriptSelf.setProperty(it.name(), it.value());
        }
    }

    m_scriptSelf.setProperty("QRectF", constructQRectFClass(this));
    m_scriptSelf.setProperty("Activity", newFunction(ScriptEngine::newActivity));
    m_scriptSelf.setProperty("Panel", newFunction(ScriptEngine::newPanel));
    m_scriptSelf.setProperty("panelById", newFunction(ScriptEngine::panelById));
    m_scriptSelf.setProperty("panels", newFunction(ScriptEngine::panels));
    m_scriptSelf.setProperty("fileExists", newFunction(ScriptEngine::fileExists));
    m_scriptSelf.setProperty("loadTemplate", newFunction(ScriptEngine::loadTemplate));
    m_scriptSelf.setProperty("applicationExists", newFunction(ScriptEngine::applicationExists));
    m_scriptSelf.setProperty("userDataPath", newFunction(ScriptEngine::userDataPath));
    m_scriptSelf.setProperty("applicationPath", newFunction(ScriptEngine::applicationPath));
    m_scriptSelf.setProperty("knownWallpaperPlugins", newFunction(ScriptEngine::knownWallpaperPlugins));
    m_scriptSelf.setProperty("ConfigFile", newFunction(ScriptEngine::configFile));

    setGlobalObject(m_scriptSelf);
}

bool ScriptEngine::isPanel(const Plasma::Containment *c)
{
    if (!c) {
        return false;
    }

    return c->containmentType() == Plasma::Containment::PanelContainment ||
           c->containmentType() == Plasma::Containment::CustomPanelContainment;
}

Plasma::Corona *ScriptEngine::corona() const
{
    return m_corona;
}

bool ScriptEngine::evaluateScript(const QString &script, const QString &path)
{
    //kDebug() << "evaluating" << m_editor->toPlainText();
    evaluate(script, path);
    if (hasUncaughtException()) {
        //kDebug() << "catch the exception!";
        QString error = i18n("Error: %1 at line %2\n\nBacktrace:\n%3",
                             uncaughtException().toString(),
                             QString::number(uncaughtExceptionLineNumber()),
                             uncaughtExceptionBacktrace().join("\n  "));
        emit printError(error);
        return false;
    }

    return true;
}

void ScriptEngine::exception(const QScriptValue &value)
{
    //kDebug() << "exception caught!" << value.toVariant();
    emit printError(value.toVariant().toString());
}

}

#include "moc_scriptengine.cpp"

