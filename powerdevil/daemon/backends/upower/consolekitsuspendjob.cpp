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

#include "consolekitsuspendjob.h"

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QTimer>
#include <KDebug>
#include <KLocale>

ConsoleKitSuspendJob::ConsoleKitSuspendJob(QDBusInterface *consolekitInterface,
                                   PowerDevil::BackendInterface::SuspendMethod method,
                                   PowerDevil::BackendInterface::SuspendMethods supported)
    : KJob(), m_consolekitInterface(consolekitInterface)
{
    kDebug() << "Starting ConsoleKit suspend job";
    m_method = method;
    m_supported = supported;

    connect(m_consolekitInterface, SIGNAL(PrepareForSleep(bool)), this, SLOT(slotConsoleKitResuming(bool)));
}

ConsoleKitSuspendJob::~ConsoleKitSuspendJob()
{

}

void ConsoleKitSuspendJob::start()
{
    QTimer::singleShot(0, this, SLOT(doStart()));
}

void ConsoleKitSuspendJob::kill(bool /*quietly */)
{

}

void ConsoleKitSuspendJob::doStart()
{
    if (m_supported & m_method)
    {
        QVariantList args;
        args << true; // interactive, ie. with polkit dialogs

        QDBusPendingReply<void> reply;
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(sendResult(QDBusPendingCallWatcher*)));

        switch(m_method)
        {
        case PowerDevil::BackendInterface::ToRam:
            reply = m_consolekitInterface->asyncCallWithArgumentList("Suspend", args);
            break;
        case PowerDevil::BackendInterface::ToDisk:
            reply = m_consolekitInterface->asyncCallWithArgumentList("Hibernate", args);
            break;
        case PowerDevil::BackendInterface::HybridSuspend:
            reply = m_consolekitInterface->asyncCallWithArgumentList("HybridSleep", args);
            break;
        default:
            kDebug() << "Unsupported suspend method";
            setError(1);
            setErrorText(i18n("Unsupported suspend method"));
            break;
        }
    }
}

void ConsoleKitSuspendJob::sendResult(QDBusPendingCallWatcher *watcher)
{
    const QDBusPendingReply<void> reply = *watcher;
    if (!reply.isError()) {
        emitResult();
    } else {
        kWarning() << "Failed to start suspend job" << reply.error().name() << reply.error().message();
    }

    watcher->deleteLater();
}

void ConsoleKitSuspendJob::slotConsoleKitResuming(bool active)
{
    if (!active)
        emitResult();
}


#include "moc_consolekitsuspendjob.cpp"
