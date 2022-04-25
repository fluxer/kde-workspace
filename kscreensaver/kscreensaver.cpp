/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kscreensaver.h"
#include "screensaveradaptor.h"

#include <kdebug.h>
#include <kidletime.h>

#include <sys/types.h>
#include <unistd.h>

// TODO: fallback to ConsoleKit

KScreenSaver::KScreenSaver(QObject *parent)
    : QObject(parent),
    m_objectsregistered(false),
    m_serviceregistered(false),
    m_xscreensaver(nullptr),
    m_login1("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus())
{
    (void)new ScreenSaverAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();

    const bool object = connection.registerObject("/ScreenSaver", this); // used by e.g. xdg-screensaver
    if (!object) {
        kWarning() << "Could not register object" << connection.lastError().message();
        return;
    }
    const bool object2 = connection.registerObject("/org/freedesktop/ScreenSaver", this); // used by e.g. chromium
    if (!object2) {
        kWarning() << "Could not register second object" << connection.lastError().message();
        connection.unregisterObject("/ScreenSaver");
        return;
    }
    m_objectsregistered = true;

    const bool service = connection.registerService("org.freedesktop.ScreenSaver");
    if (!service) {
        kWarning() << "Could not register service" << connection.lastError().message();
        connection.unregisterObject("/ScreenSaver");
        connection.unregisterObject("/org/freedesktop/ScreenSaver");
        return;
    }
    m_serviceregistered = true;

    QProcess::startDetached(
        QString::fromLatin1("xscreensaver"),
        QStringList() << QString::fromLatin1("-nosplash")
    );

    m_xscreensaver = new QProcess(this);
    connect(m_xscreensaver, SIGNAL(readyReadStandardOutput()), this, SLOT(slotXScreenSaverOutput()));
    connect(m_xscreensaver, SIGNAL(readyReadStandardError()), this, SLOT(slotXScreenSaverError()));
    m_xscreensaver->start(
        QString::fromLatin1("xscreensaver-command"),
        QStringList() << QString::fromLatin1("-watch")
    );

    if (m_login1.isValid()) {
        QDBusReply<QDBusObjectPath> reply = m_login1.call("GetSessionByPID", uint(::getpid()));
        if (reply.isValid()) {
            connection = QDBusConnection::systemBus();
            const QString login1sessionpath = reply.value().path();
            // qDebug() << Q_FUNC_INFO << login1sessionpath;
            connection.connect(
                "org.freedesktop.login1", login1sessionpath, "org.freedesktop.login1.Session", "Lock",
                this, SLOT(slotLock())
            );
            connection.connect(
                "org.freedesktop.login1", login1sessionpath, "org.freedesktop.login1.Session", "Unlock",
                this, SLOT(slotUnlock())
            );
        } else {
            kWarning() << "Invalid GetSessionByPID reply";
        }
    }
}

KScreenSaver::~KScreenSaver()
{
    if (m_serviceregistered) {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterService("org.freedesktop.ScreenSaver");
    }

    if (m_objectsregistered) {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterObject("/ScreenSaver");
        connection.unregisterObject("/org/freedesktop/ScreenSaver");
    }

    if (m_xscreensaver) {
        disconnect(m_xscreensaver, SIGNAL(readyReadStandardOutput()), this, SLOT(slotXScreenSaverOutput()));
        disconnect(m_xscreensaver, SIGNAL(readyReadStandardError()), this, SLOT(slotXScreenSaverError()));
        m_xscreensaver->deleteLater();
    }
}

bool KScreenSaver::GetActive()
{
    // qDebug() << Q_FUNC_INFO;
    return (GetActiveTime() > 0);
}

uint KScreenSaver::GetActiveTime()
{
    // qDebug() << Q_FUNC_INFO;
    if (!m_activetimer.isValid()) {
        return 0;
    }
    return m_activetimer.elapsed();
}

uint KScreenSaver::GetSessionIdleTime()
{
    // qDebug() << Q_FUNC_INFO;
    return KIdleTime::instance()->idleTime();
}

void KScreenSaver::Lock()
{
    // qDebug() << Q_FUNC_INFO;
    const int xscreensaverstatus = QProcess::execute(
        QString::fromLatin1("xscreensaver-command"),
        QStringList() << QString::fromLatin1("-lock")
    );
    if (xscreensaverstatus != 0) {
        kWarning() << "Could not lock";
    }
}

bool KScreenSaver::SetActive(bool active)
{
    // qDebug() << Q_FUNC_INFO << active;
    int xscreensaverstatus = false;
    if (active) {
        xscreensaverstatus = QProcess::execute(
            QString::fromLatin1("xscreensaver-command"),
            QStringList() << QString::fromLatin1("-activate")
        );
    } else {
        xscreensaverstatus = QProcess::execute(
            QString::fromLatin1("xscreensaver-command"),
            QStringList() << QString::fromLatin1("-deactivate")
        );
    }
    if (xscreensaverstatus != 0) {
        kWarning() << "Could not set activity";
    }
    return xscreensaverstatus;
}

void KScreenSaver::SimulateUserActivity()
{
    // qDebug() << Q_FUNC_INFO;
    KIdleTime::instance()->simulateUserActivity();
}

uint KScreenSaver::Inhibit(const QString &application_name, const QString &reason_for_inhibit)
{
    // qDebug() << Q_FUNC_INFO << application_name << reason_for_inhibit;
    QDBusInterface poweriface(
        "org.freedesktop.PowerManagement",
        "/org/freedesktop/PowerManagement/Inhibit",
        "org.freedesktop.PowerManagement.Inhibit",
        QDBusConnection::sessionBus()
    );
    QDBusReply<uint> powerreply = poweriface.call("Inhibit", application_name, reason_for_inhibit);
    if (powerreply.isValid()) {
        uint inhibitioncookie = powerreply.value();
        m_inhibitions.append(inhibitioncookie);
        return inhibitioncookie;
    }
    kWarning() << "Power manager reply is invalid";
    return 0;
}

void KScreenSaver::UnInhibit(uint cookie)
{
    // qDebug() << Q_FUNC_INFO << cookie;
    if (m_inhibitions.contains(cookie)) {
        QDBusInterface poweriface(
            "org.freedesktop.PowerManagement",
            "/org/freedesktop/PowerManagement/Inhibit",
            "org.freedesktop.PowerManagement.Inhibit",
            QDBusConnection::sessionBus()
        );
        poweriface.asyncCall("UnInhibit", cookie);
        m_inhibitions.removeAll(cookie);
    }
}

void KScreenSaver::slotXScreenSaverOutput()
{
    // qDebug() << Q_FUNC_INFO;
    const QByteArray xscreensaverdata = m_xscreensaver->readAllStandardOutput();
    foreach (const QByteArray &xscreensaverline, xscreensaverdata.split('\n')) {
        // qDebug() << Q_FUNC_INFO << xscreensaverline;
        if (xscreensaverline.isEmpty()) {
            continue;
        }

        if (xscreensaverline.startsWith("BLANK") || xscreensaverline.startsWith("LOCK")) {
            m_activetimer.restart();
            emit ActiveChanged(true);
        } else if (xscreensaverline.startsWith("UNBLANK")) {
            m_activetimer.invalidate();
            emit ActiveChanged(false);
        }
    }
}

void KScreenSaver::slotXScreenSaverError()
{
    // qDebug() << Q_FUNC_INFO;
    const QByteArray xscreensaverdata = m_xscreensaver->readAllStandardError();
    kWarning() << xscreensaverdata;
}

void KScreenSaver::slotLock()
{
    // qDebug() << Q_FUNC_INFO;
    Lock();
}

void KScreenSaver::slotUnlock()
{
    // qDebug() << Q_FUNC_INFO;
    SetActive(false);
}

#include "moc_kscreensaver.cpp"
