/*
 *   Copyright 2007-2008 Richard J. Moore <rich@kde.org>
 *   Copyright 2009 Aaron J. Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#include "scriptenv.h"

#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QtCore/qmetaobject.h>

#include <KDebug>
#include <KDesktopFile>
#include <KIO/Job>
#include <KLocale>
#include <KMimeType>
#include <KPluginInfo>
#include <KService>
#include <KServiceTypeTrader>
#include <KShell>
#include <KStandardDirs>
#include <KRun>

#include <Plasma/Package>

#include "simplebindings/filedialogproxy.h"

Q_DECLARE_METATYPE(ScriptEnv*)

ScriptEnv::ScriptEnv(QObject *parent, QScriptEngine *engine)
    : QObject(parent),
    m_engine(engine)
{
    connect(m_engine, SIGNAL(signalHandlerException(QScriptValue)), this, SLOT(signalException()));

    setupGlobalObject();
}

ScriptEnv::~ScriptEnv()
{
}

void ScriptEnv::setupGlobalObject()
{
    QScriptValue global = m_engine->globalObject();

    // Add an accessor so we can find the scriptenv given only the engine. The
    // property is hidden from scripts.
    global.setProperty("__plasma_scriptenv", m_engine->newQObject(this),
                       QScriptValue::ReadOnly|QScriptValue::Undeletable|QScriptValue::SkipInEnumeration);
    // Add utility functions
    global.setProperty("debug", m_engine->newFunction(ScriptEnv::debug));
}

void ScriptEnv::addMainObjectProperties(QScriptValue &value)
{
    value.setProperty("addEventListener", m_engine->newFunction(ScriptEnv::addEventListener));
    value.setProperty("removeEventListener", m_engine->newFunction(ScriptEnv::removeEventListener));
}

QScriptEngine *ScriptEnv::engine() const
{
    return m_engine;
}

ScriptEnv *ScriptEnv::findScriptEnv(QScriptEngine *engine)
{
    QScriptValue global = engine->globalObject();
    return qscriptvalue_cast<ScriptEnv*>(global.property("__plasma_scriptenv"));
}

void ScriptEnv::signalException()
{
    checkForErrors(false);
}

void ScriptEnv::registerEnums(QScriptValue &scriptValue, const QMetaObject &meta)
{
    //manually create enum values. ugh
    QScriptEngine *engine = scriptValue.engine();
    for (int i = 0; i < meta.enumeratorCount(); ++i) {
        QMetaEnum e = meta.enumerator(i);
        //kDebug() << e.name();
        for (int i=0; i < e.keyCount(); ++i) {
            //kDebug() << e.key(i) << e.value(i);
            scriptValue.setProperty(e.key(i), QScriptValue(engine, e.value(i)));
        }
    }
}

bool ScriptEnv::include(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kWarning() << i18n("Unable to load script file: %1", path);
        return false;
    }

    QString script = file.readAll();
    //kDebug() << "Script says" << script;

    // change the context to the parent context so that the include is actually
    // executed in the same context as the caller; seems to be what javascript
    // coders expect :)
    QScriptContext *ctx = m_engine->currentContext();
    if (ctx && ctx->parentContext()) {
        ctx->setActivationObject(ctx->parentContext()->activationObject());
        ctx->setThisObject(ctx->parentContext()->thisObject());
    }

    m_engine->evaluate(script, path);

    return !checkForErrors(true);
}

bool ScriptEnv::checkForErrors(bool fatal)
{
    if (m_engine->hasUncaughtException()) {
        emit reportError(this, fatal);
        if (!fatal) {
            m_engine->clearExceptions();
        }
        return true;
    }

    return false;
}

QScriptValue ScriptEnv::debug(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1) {
        return throwNonFatalError(i18n("debug takes one argument"), context, engine);
    }

    kDebug() << context->argument(0).toString();
    return engine->undefinedValue();
}

QScriptValue ScriptEnv::throwNonFatalError(const QString &msg, QScriptContext *context, QScriptEngine *engine)
{
    QScriptValue rv = context->throwError(msg);
    ScriptEnv *env = ScriptEnv::findScriptEnv(engine);
    if (env) {
        env->checkForErrors(false);
    }
    return rv;
}

QString ScriptEnv::filePathFromScriptContext(const char *type, const QString &file) const
{
    //kDebug() << type << file;
    QScriptContext *c = m_engine->currentContext();
    while (c) {
        QScriptValue v = c->activationObject().property("__plasma_package");
        //kDebug() << "variant in parent context?" << v.isVariant();
        if (v.isVariant()) {
            const QString path = v.toVariant().value<Plasma::Package>().filePath(type, file);
            if (!path.isEmpty()) {
                return path;
            }
        }

        c = c->parentContext();
    }

    //kDebug() << "fail";
    return QString();
}

QScriptValue ScriptEnv::addEventListener(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() < 2) {
        return false;
    }

    ScriptEnv *env = ScriptEnv::findScriptEnv(engine);
    if (!env) {
        return false;
    }

    return env->addEventListener(context->argument(0).toString(), context->argument(1));
}

QScriptValue ScriptEnv::removeEventListener(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() < 2) {
        return false;
    }

    ScriptEnv *env = ScriptEnv::findScriptEnv(engine);
    if (!env) {
        return false;
    }

    return env->removeEventListener(context->argument(0).toString(), context->argument(1));
}

QScriptValue ScriptEnv::callFunction(QScriptValue &func, const QScriptValueList &args, const QScriptValue &activator)
{
    if (!func.isFunction()) {
        return m_engine->undefinedValue();
    }

    QScriptContext *ctx = m_engine->pushContext();
    ctx->setActivationObject(activator);
    QScriptValue rv = func.call(activator, args);
    m_engine->popContext();

    if (m_engine->hasUncaughtException()) {
        emit reportError(this, false);
        m_engine->clearExceptions();
        return m_engine->undefinedValue();
    }

    return rv;
}

bool ScriptEnv::hasEventListeners(const QString &event) const
{
    return m_eventListeners.contains(event);
}

bool ScriptEnv::callEventListeners(const QString &event, const QScriptValueList &args)
{
    if (!m_eventListeners.contains(event.toLower())) {
        return false;
    }

    QScriptValueList funcs = m_eventListeners.value(event.toLower());
    QMutableListIterator<QScriptValue> it(funcs);
    while (it.hasNext()) {
        callFunction(it.next(), args);
    }

    return true;
}

bool ScriptEnv::addEventListener(const QString &event, const QScriptValue &func)
{
    if (func.isFunction() && !event.isEmpty()) {
        m_eventListeners[event.toLower()].append(func);
        return true;
    }

    return false;
}

bool ScriptEnv::removeEventListener(const QString &event, const QScriptValue &func)
{
    bool found = false;

    if (func.isFunction()) {
        QScriptValueList funcs = m_eventListeners.value(event);
        QMutableListIterator<QScriptValue> it(funcs);
        while (it.hasNext()) {
            if (it.next().equals(func)) {
                it.remove();
                found = true;
            }
        }

        if (funcs.isEmpty()) {
            m_eventListeners.remove(event.toLower());
        } else {
            m_eventListeners.insert(event.toLower(), funcs);
        }
    }

    return found;
}

#include "moc_scriptenv.cpp"
