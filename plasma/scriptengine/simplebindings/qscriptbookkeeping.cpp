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

#include <QGraphicsWidget>
#include <QScriptEngine>

#include <KConfigGroup>
#include <KIO/Job>
#include <KSharedConfig>

#include <Plasma/Applet>
#include <Plasma/Animation>
#include <Plasma/Extender>

#include "dataengine.h"

Q_DECLARE_METATYPE(KConfigGroup)
Q_DECLARE_METATYPE(KJob *)
Q_DECLARE_METATYPE(KIO::Job *)

Q_DECLARE_METATYPE(QGraphicsWidget*)
Q_DECLARE_METATYPE(QGraphicsLayout*)

Q_DECLARE_METATYPE(Plasma::Animation*)
Q_DECLARE_METATYPE(Plasma::Applet*)
Q_DECLARE_METATYPE(Plasma::Extender*)
Q_DECLARE_METATYPE(Plasma::Svg*)
Q_DECLARE_METATYPE(Qt::MouseButton)
Q_DECLARE_METATYPE(QList<double>)

typedef KJob* KJobPtr;
QScriptValue qScriptValueFromKJob(QScriptEngine *engine, const KJobPtr &job)
{
    return engine->newQObject(const_cast<KJob *>(job), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void qKJobFromQScriptValue(const QScriptValue &scriptValue, KJobPtr &job)
{
    QObject *obj = scriptValue.toQObject();
    job = static_cast<KJob *>(obj);
}

typedef KIO::Job* KioJobPtr;
QScriptValue qScriptValueFromKIOJob(QScriptEngine *engine, const KioJobPtr &job)
{
    return engine->newQObject(const_cast<KIO::Job *>(job), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void qKIOJobFromQScriptValue(const QScriptValue &scriptValue, KioJobPtr &job)
{
    QObject *obj = scriptValue.toQObject();
    job = static_cast<KIO::Job *>(obj);
}

QScriptValue qScriptValueFromKConfigGroup(QScriptEngine *engine, const KConfigGroup &config)
{
    QScriptValue obj = engine->newObject();

    if (!config.isValid()) {
        return obj;
    }

    QMap<QString, QString> entryMap = config.entryMap();
    QMap<QString, QString>::const_iterator it = entryMap.constBegin();
    QMap<QString, QString>::const_iterator begin = it;
    QMap<QString, QString>::const_iterator end = entryMap.constEnd();

    //setting the group name
    obj.setProperty("__file", QScriptValue(engine, config.config()->name()));
    obj.setProperty("__name", QScriptValue(engine, config.name()));

    //setting the key/value pairs
    for (it = begin; it != end; ++it) {
        //kDebug() << "setting" << it.key() << "to" << it.value();
        QString prop = it.key();
        prop.replace(' ', '_');
        obj.setProperty(prop, it.value());
    }

    return obj;
}

void kConfigGroupFromScriptValue(const QScriptValue& obj, KConfigGroup &config)
{
    config = KConfigGroup(KSharedConfig::openConfig(obj.property("__file").toString()), obj.property("__name").toString());

    QScriptValueIterator it(obj);

    while (it.hasNext()) {
        it.next();
        //kDebug() << it.name() << "is" << it.value().toString();
        if (it.name() != "__name") {
            config.writeEntry(it.name(), it.value().toString());
        }
    }
}

typedef Plasma::Animation* AnimationPtr;
QScriptValue qScriptValueFromAnimation(QScriptEngine *engine, const AnimationPtr &anim)
{
    return engine->newQObject(const_cast<Plasma::Animation *>(anim), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void abstractAnimationFromQScriptValue(const QScriptValue &scriptValue, AnimationPtr &anim)
{
    QObject *obj = scriptValue.toQObject();
    anim = static_cast<Plasma::Animation *>(obj);
}

typedef QGraphicsWidget * QGraphicsWidgetPtr;
QScriptValue qScriptValueFromQGraphicsWidget(QScriptEngine *engine, const QGraphicsWidgetPtr &anim)
{
    return engine->newQObject(const_cast<QGraphicsWidget *>(anim), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void graphicsWidgetFromQScriptValue(const QScriptValue &scriptValue, QGraphicsWidgetPtr &anim)
{
    QObject *obj = scriptValue.toQObject();
    anim = static_cast<QGraphicsWidget *>(obj);
}

typedef Plasma::Svg * SvgPtr;
QScriptValue qScriptValueFromSvg(QScriptEngine *engine, const SvgPtr &anim)
{
    return engine->newQObject(const_cast<Plasma::Svg *>(anim), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void svgFromQScriptValue(const QScriptValue &scriptValue, SvgPtr &anim)
{
    QObject *obj = scriptValue.toQObject();
    anim = static_cast<Plasma::Svg *>(obj);
}

typedef Plasma::Extender *ExtenderPtr;
QScriptValue qScriptValueFromExtender(QScriptEngine *engine, const ExtenderPtr &extender)
{
    return engine->newQObject(const_cast<Plasma::Extender *>(extender), QScriptEngine::AutoOwnership, QScriptEngine::PreferExistingWrapperObject);
}

void extenderFromQScriptValue(const QScriptValue &scriptValue, ExtenderPtr &extender)
{
    QObject *obj = scriptValue.toQObject();
    extender = static_cast<Plasma::Extender *>(obj);
}

QScriptValue qScriptValueFromMouseButton(QScriptEngine *, const Qt::MouseButton &button)
{
    return int(button);
}

void mouseButtonFromScriptValue(const QScriptValue &scriptValue, Qt::MouseButton &button)
{
    button = static_cast<Qt::MouseButton>(scriptValue.toInt32());
}

void registerSimpleAppletMetaTypes(QScriptEngine *engine)
{
    qScriptRegisterMetaType<KConfigGroup>(engine, qScriptValueFromKConfigGroup, kConfigGroupFromScriptValue);
    qScriptRegisterMetaType<KJob *>(engine, qScriptValueFromKJob, qKJobFromQScriptValue);
    qScriptRegisterMetaType<KIO::Job *>(engine, qScriptValueFromKIOJob, qKIOJobFromQScriptValue);
    registerDataEngineMetaTypes(engine);

    qScriptRegisterMetaType<QGraphicsWidget*>(engine, qScriptValueFromQGraphicsWidget, graphicsWidgetFromQScriptValue);
    qScriptRegisterMetaType<Plasma::Svg*>(engine, qScriptValueFromSvg, svgFromQScriptValue);

    qScriptRegisterSequenceMetaType<QList<double> >(engine);
    qScriptRegisterMetaType<Plasma::Animation *>(engine, qScriptValueFromAnimation, abstractAnimationFromQScriptValue);
    qScriptRegisterMetaType<Plasma::Extender *>(engine, qScriptValueFromExtender, extenderFromQScriptValue);
    qScriptRegisterMetaType<Qt::MouseButton>(engine, qScriptValueFromMouseButton, mouseButtonFromScriptValue);
}


