/*  This file is part of the KDE project

    Copyright (c) 2010 Klarälvdalens Datakonsult AB,
                       a KDAB Group company <info@kdab.com>
    Author: Kevin Ottens <kevin.ottens@kdab.com>

    Copyright (c) 2011 Lukas Tinkl <ltinkl@redhat.com>
    Copyright (c) 2011-2012 Lamarque V. Souza <lamarque@kde.org>

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

#include "networkmanagerstatus.h"

#include <QtDBus/QDBusReply>

#include <KDebug>

#define NM_DBUS_SERVICE "org.freedesktop.NetworkManager"
#define NM_DBUS_PATH "/org/freedesktop/NetworkManager"
#define NM_DBUS_INTERFACE "org.freedesktop.NetworkManager"

NetworkManagerStatus::NetworkManagerStatus(QObject *parent)
    : SystemStatusInterface(parent),
    m_status(Solid::Networking::Unknown),
    m_nm(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, QDBusConnection::systemBus())
{
    if (isSupported()) {
        connect(&m_nm, SIGNAL(StateChanged(uint)), this, SLOT(nmStateChanged(uint)));

        QDBusReply<uint> reply = m_nm.call("state");
        if (reply.isValid()) {
            nmStateChanged(reply.value());
        }
    }
}

Solid::Networking::Status NetworkManagerStatus::status() const
{
    return m_status;
}

bool NetworkManagerStatus::isSupported() const
{
    return m_nm.isValid();
}

QString NetworkManagerStatus::serviceName() const
{
    return QString::fromLatin1(NM_DBUS_SERVICE);
}

void NetworkManagerStatus::nmStateChanged(uint nmState)
{
    m_status = Solid::Networking::Unknown;
    // for reference:
    // https://developer-old.gnome.org/NetworkManager/stable/gdbus-org.freedesktop.NetworkManager.html
    switch (nmState) {
    case 0:
    case 10:
        break;
    case 20:
        m_status = Solid::Networking::Unconnected;
        break;
    case 30:
        m_status = Solid::Networking::Disconnecting;
        break;
    case 40:
        m_status = Solid::Networking::Connecting;
        break;
    case 50:
    case 60:
    case 70:
        m_status = Solid::Networking::Connected;
        break;
    default:
        kWarning() << "unknown state" << nmState;
    }
    Q_EMIT statusChanged(m_status);
}

#include "moc_networkmanagerstatus.cpp"
