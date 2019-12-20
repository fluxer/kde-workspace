/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2013 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
// own
#include "client_machine.h"
// KWin
#include "utils.h"
// KDE
#include <KDebug>
// Qt
#include <QHostInfo>

namespace KWin {

ClientMachine::ClientMachine(QObject *parent)
    : QObject(parent)
    , m_localhost(false)
    , m_resolved(false)
{
}

ClientMachine::~ClientMachine()
{
}

void ClientMachine::resolve(xcb_window_t window, xcb_window_t clientLeader)
{
    if (m_resolved) {
        return;
    }
    QByteArray name = getStringProperty(window, XCB_ATOM_WM_CLIENT_MACHINE);
    if (name.isEmpty() && clientLeader && clientLeader != window) {
        name = getStringProperty(clientLeader, XCB_ATOM_WM_CLIENT_MACHINE);
    }
    if (name.isEmpty()) {
        name = localhost();
    }
    if (name == localhost()) {
        setLocal();
    }
    m_hostName = name;
    checkForLocalhost();
    m_resolved = true;
}

void ClientMachine::checkForLocalhost()
{
    if (isLocal()) {
        // nothing to do
        return;
    }
    QByteArray host = QHostInfo::localHostName().toAscii();

    if (!host.isEmpty()) {
        host = host.toLower();
        const QByteArray lowerHostName(m_hostName.toLower());
        if (host == lowerHostName) {
            setLocal();
            return;
        }
        if (char *dot = strchr(host.data(), '.')) {
            *dot = '\0';
            if (host == lowerHostName) {
                setLocal();
            }
        }
    }
}

void ClientMachine::setLocal()
{
    m_localhost = true;
    emit localhostChanged();
}

} // namespace
