/*  This file is part of the KDE project

    Copyright (c) 2021 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "connmanstatus.h"

#include <QtDBus/QDBusReply>

#include <KDebug>

#define CONNMAN_DBUS_SERVICE "net.connman"
#define CONNMAN_DBUS_PATH "/"
#define CONNMAN_DBUS_INTERFACE "net.connman.Manager"

typedef QMap<QString,QVariant> ConnmanPropertiesType;

ConnmanStatus::ConnmanStatus(QObject *parent)
    : SystemStatusInterface(parent),
    m_status(Solid::Networking::Unknown),
    m_connman(CONNMAN_DBUS_SERVICE, CONNMAN_DBUS_PATH, CONNMAN_DBUS_INTERFACE, QDBusConnection::systemBus())
{
    if (isSupported()) {
        connect(&m_connman, SIGNAL(PropertyChanged()), this, SLOT(connmanStateChanged()));
        connmanStateChanged();
    }
}

Solid::Networking::Status ConnmanStatus::status() const
{
    return m_status;
}

bool ConnmanStatus::isSupported() const
{
    return m_connman.isValid();
}

QString ConnmanStatus::serviceName() const
{
    return QString::fromLatin1(CONNMAN_DBUS_SERVICE);
}

void ConnmanStatus::connmanStateChanged()
{
    m_status = Solid::Networking::Unknown;
    QDBusReply<ConnmanPropertiesType> reply = m_connman.call("GetProperties");
    if (!reply.isValid()) {
        kWarning() << "invalid reply" << reply.error().message();
    } else {
        const ConnmanPropertiesType props = reply.value();
        const QString state = props.value("State").toString();
        // for reference:
        // https://git.kernel.org/pub/scm/network/connman/connman.git/tree/doc/overview-api.txt
        if (state == QLatin1String("ready") || state == QLatin1String("association")
            || state == QLatin1String("configuration")) {
            m_status = Solid::Networking::Connecting;
        } else if (state == QLatin1String("online")) {
            m_status = Solid::Networking::Connected;
        } else if (state == QLatin1String("disconnect")) {
            m_status = Solid::Networking::Disconnecting;
        } else if (state == QLatin1String("offline") || state == QLatin1String("idle")) {
            m_status = Solid::Networking::Unconnected;
        } else {
            kWarning() << "unknown state" << state;
        } 
    }

    Q_EMIT statusChanged(m_status);
}

#include "moc_connmanstatus.cpp"
