/*  This file is part of the KDE project
    Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef CONSOLEKITSUSPENDJOB_H
#define CONSOLEKITSUSPENDJOB_H

#include <kjob.h>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/qdbuspendingcall.h>

#include "powerdevilbackendinterface.h"

class ConsoleKitSuspendJob : public KJob
{
    Q_OBJECT
public:
    ConsoleKitSuspendJob(QDBusInterface *consolekitInterface,
                     PowerDevil::BackendInterface::SuspendMethod method,
                     PowerDevil::BackendInterface::SuspendMethods supported);
    virtual ~ConsoleKitSuspendJob();

    void start();
    void kill(bool quietly);

private Q_SLOTS:
    void doStart();
    void sendResult(QDBusPendingCallWatcher* watcher);
    void slotConsoleKitResuming(bool active);

private:
    QDBusInterface *m_consolekitInterface;
    PowerDevil::BackendInterface::SuspendMethod m_method;
    PowerDevil::BackendInterface::SuspendMethods m_supported;
};

#endif //CONSOLEKITSUSPENDJOB_H
