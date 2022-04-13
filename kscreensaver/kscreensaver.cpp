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

static const uint ChangeScreenSettings = 4;

KScreenSaver::KScreenSaver(QObject *parent)
    : QObject(parent),
    m_xscreensaver(nullptr),
    m_inhibitionscounter(0)
{
    (void)new ScreenSaverAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();
    const bool object = connection.registerObject("/ScreenSaver", this);
    if (object) {
        const bool service = connection.registerService("org.freedesktop.ScreenSaver");
        if (service) {
            kDebug() << "kscreensaver is online";
        } else {
            kWarning() << "Could not register service" << connection.lastError().message();
            return;
        }
    } else {
        kWarning() << "Could not register object" << connection.lastError().message();
        return;
    }

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
}

KScreenSaver::~KScreenSaver()
{
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
    const bool xscreensaverstatus = QProcess::execute(
        QString::fromLatin1("xscreensaver-command"),
        QStringList() << QString::fromLatin1("-lock")
    );
    if (!xscreensaverstatus) {
        kWarning() << "Could not lock";
    }
}

bool KScreenSaver::SetActive(bool active)
{
    // qDebug() << Q_FUNC_INFO << active;
    bool xscreensaverstatus = false;
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
    if (!xscreensaverstatus) {
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
    m_inhibitionscounter++;
    QDBusInterface policyAgent(
        "org.kde.Solid.PowerManagement",
        "/org/kde/Solid/PowerManagement/PolicyAgent",
        "org.kde.Solid.PowerManagement.PolicyAgent",
        QDBusConnection::sessionBus()
    );
    policyAgent.asyncCall("AddInhibition", ChangeScreenSettings, application_name, reason_for_inhibit);
    m_inhibitions.append(m_inhibitionscounter);
    return m_inhibitionscounter;
}

uint KScreenSaver::Throttle(const QString &application_name, const QString &reason_for_inhibit)
{
    // qDebug() << Q_FUNC_INFO << application_name << reason_for_inhibit;
    // was not implemented before either
    Q_UNUSED(application_name);
    Q_UNUSED(reason_for_inhibit);
    return 0;
}

void KScreenSaver::UnInhibit(uint cookie)
{
    // qDebug() << Q_FUNC_INFO << cookie;
    if (m_inhibitions.contains(cookie)) {
        QDBusInterface policyAgent(
            "org.kde.Solid.PowerManagement",
            "/org/kde/Solid/PowerManagement/PolicyAgent",
            "org.kde.Solid.PowerManagement.PolicyAgent",
            QDBusConnection::sessionBus()
        );
        policyAgent.asyncCall("ReleaseInhibition", cookie);
        m_inhibitions.removeAll(cookie);
    }
}

void KScreenSaver::UnThrottle(uint cookie)
{
    // qDebug() << Q_FUNC_INFO << cookie;
    // was not implemented before either
    Q_UNUSED(cookie);
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

#include "moc_kscreensaver.cpp"
