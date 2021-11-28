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

#include "toolkitstatus.h"

#include <QNetworkInterface>
#include <KDebug>

ToolkitStatus::ToolkitStatus(QObject *parent)
    : SystemStatusInterface(parent),
    m_status(Solid::Networking::Unknown),
    m_timer(this)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
    m_timer.start(2000);
}

Solid::Networking::Status ToolkitStatus::status() const
{
    return m_status;
}

bool ToolkitStatus::isSupported() const
{
    return true;
}

QString ToolkitStatus::serviceName() const
{
    return QString::fromLatin1("org.kde.kded");
}

void ToolkitStatus::checkStatus()
{
    Solid::Networking::Status newstatus = Solid::Networking::Unconnected;
    Q_FOREACH (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
        const QNetworkInterface::InterfaceFlags iflags = iface.flags();
        if (iflags & QNetworkInterface::CanBroadcast && iflags & QNetworkInterface::IsRunning) {
            newstatus = Solid::Networking::Connected;
            break;
        }
    }
    // qDebug() << Q_FUNC_INFO << m_status << newstatus;

    if (m_status != newstatus) {
        m_status = newstatus;
        Q_EMIT statusChanged(m_status);
    }
}

#include "moc_toolkitstatus.cpp"
