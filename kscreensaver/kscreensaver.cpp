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
#include "config-X11.h"

#include <QX11Info>
#include <kuser.h>
#include <kidletime.h>
#include <kdebug.h>

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#ifdef HAVE_DPMS
#  include <X11/Xlib.h>
#  include <X11/extensions/dpms.h>
#endif

KScreenSaver::KScreenSaver(QObject *parent)
    : QObject(parent),
    m_objectsregistered(false),
    m_serviceregistered(false),
    m_havedpms(false),
    m_dpmsactive(false),
    m_statetimer(this),
    m_login1("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus()),
    m_consolekit("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager", QDBusConnection::systemBus())
{
    (void)new ScreenSaverAdaptor(this);

    m_activetimer.invalidate();

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
    } else if (m_consolekit.isValid()) {
        QDBusReply<QDBusObjectPath> reply = m_consolekit.call("GetSessionByPID", uint(::getpid()));
        if (reply.isValid()) {
            connection = QDBusConnection::systemBus();
            const QString consolekitsessionpath = reply.value().path();
            // qDebug() << Q_FUNC_INFO << consolekitsessionpath;
            connection.connect(
                "org.freedesktop.ConsoleKit", consolekitsessionpath, "org.freedesktop.ConsoleKit.Session", "Lock",
                this, SLOT(slotLock())
            );
            connection.connect(
                "org.freedesktop.ConsoleKit", consolekitsessionpath, "org.freedesktop.ConsoleKit.Session", "Unlock",
                this, SLOT(slotUnlock())
            );
        } else {
            kWarning() << "Invalid GetSessionByPID reply";
        }
    }

#ifdef HAVE_DPMS
    int dpmsevent = 0;
    int dpmserror = 0;
    int dpmstatus = DPMSQueryExtension(QX11Info::display(), &dpmsevent, &dpmserror);
    if (dpmstatus == 0) {
        kWarning() << "No DPMS extension";
        return;
    }

    dpmstatus = DPMSCapable(QX11Info::display());
    if (dpmstatus == 0) {
        kWarning() << "Not DPMS capable";
        return;
    }
    m_havedpms = true;

    connect(&m_statetimer, SIGNAL(timeout()), this, SLOT(slotCheckState()));
    m_statetimer.start(2000);
#endif // HAVE_DPMS

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
    // NOTE: this is known to work only with LightDM
    QDBusInterface dmiface(
        QString::fromLatin1("org.freedesktop.DisplayManager"),
        QString::fromLatin1("/org/freedesktop/DisplayManager"),
        QString::fromLatin1("org.freedesktop.DisplayManager"),
        QDBusConnection::systemBus()
    );
    if (!dmiface.isValid()) {
        kWarning() << "Display manager interface is not valid";
        return;
    }

    const QString username = KUser().loginName();
    const QList<QDBusObjectPath> dmsessions = qvariant_cast<QList<QDBusObjectPath>>(dmiface.property("Sessions"));
    // qDebug() << Q_FUNC_INFO << dmiface.property("Sessions");
    foreach (const QDBusObjectPath &dmsessionobj, dmsessions) {
        QDBusInterface dmsessioniface(
            QString::fromLatin1("org.freedesktop.DisplayManager"),
            dmsessionobj.path(),
            QString::fromLatin1("org.freedesktop.DisplayManager.Session"),
            QDBusConnection::systemBus()
        );
        if (!dmsessioniface.isValid()) {
            kWarning() << "Display manager session interface is not valid";
            continue;
        }
        const QString dmusername = dmsessioniface.property("UserName").toString();
        // qDebug() << Q_FUNC_INFO << dmusername << username;
        if (dmusername == username) {
            dmsessioniface.asyncCall("Lock");
            return;
        }
    }
    kWarning() << "Could not find session for" << username;
}

bool KScreenSaver::SetActive(bool active)
{
    // qDebug() << Q_FUNC_INFO << active;
    if (!m_havedpms) {
        return false;
    }

#ifdef HAVE_DPMS
    int dpmstatus = 1;
    if (active) {
        dpmstatus = DPMSForceLevel(QX11Info::display(), DPMSModeOff);
    } else {
        dpmstatus = DPMSForceLevel(QX11Info::display(), DPMSModeOn);
    }
    if (dpmstatus == 0) {
        kWarning() << "Could not set DPMS level";
        return false;
    }
    return true;
#endif
}

void KScreenSaver::SimulateUserActivity()
{
    // qDebug() << Q_FUNC_INFO;
    KIdleTime::instance()->simulateUserActivity();
}

uint KScreenSaver::Inhibit(const QString &application_name, const QString &reason_for_inhibit)
{
    // qDebug() << Q_FUNC_INFO << application_name << reason_for_inhibit;
    uint cookiecounter = 0;
    while (m_cookies.contains(cookiecounter)) {
        if (cookiecounter >= INT_MAX) {
            kWarning() << "Inhibit limit reached";
            return 0;
        }
        cookiecounter++;
    }
    m_cookies.append(cookiecounter);
    return cookiecounter;
}

void KScreenSaver::UnInhibit(uint cookie)
{
    // qDebug() << Q_FUNC_INFO << cookie;
    if (!m_cookies.contains(cookie)) {
        kWarning() << "Attempt to UnInhibit with invalid cookie";
        return;
    }
    m_cookies.removeAll(cookie);
}

void KScreenSaver::slotCheckState()
{
    // qDebug() << Q_FUNC_INFO << m_cookies;
    if (!m_havedpms) {
        return;
    }

#ifdef HAVE_DPMS
    int dpmstatus = 1;
    if (!m_cookies.isEmpty()) {
        kDebug() << "Disabling DPMS due to inhibitions";
        dpmstatus = DPMSDisable(QX11Info::display());
    } else {
        kDebug() << "Enabling DPMS due to no inhibitions";
        dpmstatus = DPMSEnable(QX11Info::display());
    }
    if (dpmstatus == 0) {
        kWarning() << "Could not set DPMS state";
    }

    CARD16 dpmslevel = 0;
    BOOL dpmsstate = false;
    dpmstatus = DPMSInfo(QX11Info::display(), &dpmslevel, &dpmsstate);
    if (dpmstatus == 0) {
        kWarning() << "Could not get DPMS info" << dpmstatus;
        return;
    }

    const bool olddpmsactive = m_dpmsactive;
    switch (dpmslevel) {
        case DPMSModeOn: {
            m_dpmsactive = false;
            break;
        }
        case DPMSModeOff:
        case DPMSModeStandby:
        case DPMSModeSuspend: {
            m_dpmsactive = true;
            break;
        }
        default: {
            kWarning() << "Unknown DPMS level" << dpmslevel;
            break;
        }
    }

    // qDebug() << Q_FUNC_INFO << dpmslevel << dpmsstate << olddpmsactive << m_dpmsactive;
    if (olddpmsactive != m_dpmsactive) {
        emit ActiveChanged(m_dpmsactive);
        if (m_dpmsactive) {
            m_activetimer.restart();
        } else {
            m_activetimer.invalidate();
        }
    }
#endif // HAVE_DPMS
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
