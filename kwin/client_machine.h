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
#ifndef KWIN_CLIENT_MACHINE_H
#define KWIN_CLIENT_MACHINE_H

#include <QObject>
#include <xcb/xcb.h>

namespace KWin {

class ClientMachine : public QObject
{
    Q_OBJECT
public:
    explicit ClientMachine(QObject *parent = NULL);
    virtual ~ClientMachine();

    void resolve(xcb_window_t window, xcb_window_t clientLeader);
    const QByteArray &hostName() const;
    bool isLocal() const;
    static QByteArray localhost();

Q_SIGNALS:
    void localhostChanged();

private Q_SLOTS:
    void setLocal();

private:
    void checkForLocalhost();
    QByteArray m_hostName;
    bool m_localhost;
    bool m_resolved;
};

inline
bool ClientMachine::isLocal() const
{
    return m_localhost;
}

inline
const QByteArray &ClientMachine::hostName() const
{
    return m_hostName;
}

inline
QByteArray ClientMachine::localhost()
{
    return "localhost";
}

} // namespace

#endif
