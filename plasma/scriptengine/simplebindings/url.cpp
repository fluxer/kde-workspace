/*
 *   Copyright 2007 Richard J. Moore <rich@kde.org>
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

#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <KUrl>
#include "backportglobal.h"

Q_DECLARE_METATYPE(KUrl*)
//Q_DECLARE_METATYPE(KUrl) unneeded; found in kurl.h

static QScriptValue urlCtor(QScriptContext *ctx, QScriptEngine *eng)
{
    if (ctx->argumentCount() == 1)
    {
        QString url = ctx->argument(0).toString();
        return qScriptValueFromValue(eng, KUrl(url));
    }

    return qScriptValueFromValue(eng, KUrl());
}

static QScriptValue urlToString(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, toString);
    return QScriptValue(eng, self->prettyUrl());
}

static QScriptValue urlProtocol(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, protocol);
    if (ctx->argumentCount()) {
        QString v = ctx->argument(0).toString();
        self->setScheme(v);
    }

    return QScriptValue(eng, self->protocol());
}

static QScriptValue urlHost(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, protocol);
    if (ctx->argumentCount()) {
        QString v = ctx->argument(0).toString();
        self->setHost(v);
    }

    return QScriptValue(eng, self->host());
}

static QScriptValue urlPath(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, path);
    if (ctx->argumentCount()) {
        QString v = ctx->argument(0).toString();
        self->setPath(v);
    }

    return QScriptValue(eng, self->path());
}

static QScriptValue urlUser(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, user);
    if (ctx->argumentCount()) {
        QString v = ctx->argument(0).toString();
        self->setUserName(v);
    }

    return QScriptValue(eng, self->userName());
}

static QScriptValue urlPassword(QScriptContext *ctx, QScriptEngine *eng)
{
    DECLARE_SELF(KUrl, password);
    if (ctx->argumentCount()) {
        QString v = ctx->argument(0).toString();
        self->setPassword(v);
    }

    return QScriptValue(eng, self->password());
}

QScriptValue constructKUrlClass(QScriptEngine *eng)
{
    QScriptValue proto = qScriptValueFromValue(eng, KUrl());
    QScriptValue::PropertyFlags getter = QScriptValue::PropertyGetter;
    QScriptValue::PropertyFlags setter = QScriptValue::PropertySetter;

    proto.setProperty("toString", eng->newFunction(urlToString), getter);
    proto.setProperty("protocol", eng->newFunction(urlProtocol), getter | setter);
    proto.setProperty("host", eng->newFunction(urlHost), getter | setter);
    proto.setProperty("path", eng->newFunction(urlPath), getter | setter);
    proto.setProperty("user", eng->newFunction(urlUser), getter | setter);
    proto.setProperty("password", eng->newFunction(urlPassword), getter | setter);

    eng->setDefaultPrototype(qMetaTypeId<KUrl*>(), proto);
    eng->setDefaultPrototype(qMetaTypeId<KUrl>(), proto);

    return eng->newFunction(urlCtor, proto);
}
