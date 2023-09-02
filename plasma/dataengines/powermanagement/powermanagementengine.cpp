/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007-2008 Sebastian Kuegler <sebas@kde.org>
 *   CopyRight 2007 Maor Vanmak <mvanmak1@gmail.com>
 *   Copyright 2008 Dario Freddi <drf54321@gmail.com>
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

#include "powermanagementengine.h"
#include "powermanagementservice.h"

#include <QDBusConnectionInterface>
#include <QDBusConnection>
#include <QDBusMetaType>

#include <KDebug>
#include <KLocale>
#include <KStandardDirs>
#include <KIdleTime>
#include <Solid/PowerManagement>
#include <Plasma/DataContainer>

typedef QMap< QString, QString > StringStringMap;
Q_DECLARE_METATYPE(StringStringMap)

PowermanagementEngine::PowermanagementEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    qDBusRegisterMetaType<StringStringMap>();

    m_sources << "Sleep States" << "PowerDevil";
}

void PowermanagementEngine::init()
{
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.freedesktop.PowerManagement")) {
        sourceRequestEvent("PowerDevil");
    }
}

QStringList PowermanagementEngine::sources() const
{
    return m_sources;
}

bool PowermanagementEngine::sourceRequestEvent(const QString &name)
{
    if (name == "Sleep States") {
        const QSet<Solid::PowerManagement::SleepState> sleepstates = Solid::PowerManagement::supportedSleepStates();
        // We first set all possible sleepstates to false, then enable the ones that are available
        setData("Sleep States", "Suspend", false);
        setData("Sleep States", "Hibernate", false);
        setData("Sleep States", "Hybrid Suspend", false);

        foreach (const Solid::PowerManagement::SleepState &sleepstate, sleepstates) {
            if (sleepstate == Solid::PowerManagement::SuspendState) {
                setData("Sleep States", "Suspend", true);
            } else if (sleepstate == Solid::PowerManagement::HibernateState) {
                setData("Sleep States", "Hibernate", true);
            } else if (sleepstate == Solid::PowerManagement::HybridSuspendState) {
                setData("Sleep States", "Hybrid Suspend", true);
            }
            //kDebug() << "Sleepstate \"" << sleepstate << "\" supported.";
        }
    } else if (name == "PowerDevil") {
        ;
    //any info concerning lock screen/screensaver goes here
    } else if (name == "UserActivity") {
        setData("UserActivity", "IdleTime", KIdleTime::instance()->idleTime());
    } else {
        kDebug() << "Data for '" << name << "' not found";
        return false;
    }
    return true;
}

bool PowermanagementEngine::updateSourceEvent(const QString &source)
{
    if (source == "UserActivity") {
        setData("UserActivity", "IdleTime", KIdleTime::instance()->idleTime());
        return true;
    }
    return Plasma::DataEngine::updateSourceEvent(source);
}

Plasma::Service* PowermanagementEngine::serviceForSource(const QString &source)
{
    if (source == "PowerDevil") {
        return new PowerManagementService(this);
    }
    return nullptr;
}

K_EXPORT_PLASMA_DATAENGINE(powermanagement, PowermanagementEngine)

#include "moc_powermanagementengine.cpp"
