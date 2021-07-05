/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>
    Copyright (C) 2008-2010 Dario Freddi <drf@kde.org>
    Copyright (C) 2010 Alejandro Fiestas <alex@eyeos.org>

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

#ifndef POWERDEVILUPOWERBACKEND_H
#define POWERDEVILUPOWERBACKEND_H

#include <config-X11.h>

#include <powerdevilbackendinterface.h>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtCore/qsharedpointer.h>

#include <kdemacros.h>

#include "upower_device_interface.h"
#include "upower_interface.h"
#include "upower_kbdbacklight_interface.h"

#define UPOWER_SERVICE "org.freedesktop.UPower"
#define UPOWER_PATH "/org/freedesktop/UPower"
#define UPOWER_IFACE "org.freedesktop.UPower"
#define UPOWER_IFACE_DEVICE "org.freedesktop.UPower.Device"

#define LOGIN1_SERVICE "org.freedesktop.login1"
#define LOGIN1_PATH "/org/freedesktop/login1"
#define LOGIN1_IFACE "org.freedesktop.login1.Manager"

#define CONSOLEKIT_SERVICE "org.freedesktop.ConsoleKit"
#define CONSOLEKIT_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CONSOLEKIT_IFACE "org.freedesktop.ConsoleKit.Manager"

class UdevHelper;
class XRandRX11Helper;
class XRandrBrightness;
class XF86VModeGamma;

class KDE_EXPORT PowerDevilUPowerBackend : public PowerDevil::BackendInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(PowerDevilUPowerBackend)
public:
    explicit PowerDevilUPowerBackend(QObject* parent);
    virtual ~PowerDevilUPowerBackend();

    virtual void init();
    static bool isAvailable();

    virtual float brightness(BrightnessControlType type = Screen) const;

    virtual void brightnessKeyPressed(PowerDevil::BackendInterface::BrightnessKeyType type, PowerDevil::BackendInterface::BrightnessControlType controlType);
    virtual bool setBrightness(float brightness, PowerDevil::BackendInterface::BrightnessControlType type = Screen);
    virtual KJob* suspend(PowerDevil::BackendInterface::SuspendMethod method);

private:
    void enumerateDevices();

private slots:
    void updateDeviceProps();
    void slotDeviceAdded(const QString &);
    void slotDeviceRemoved(const QString &);
    void slotDeviceAdded(const QDBusObjectPath & path);
    void slotDeviceRemoved(const QDBusObjectPath & path);
    void slotDeviceChanged(const QString &);
    void slotPropertyChanged();
    void slotLogin1Resuming(bool active);
    void slotConsoleKitResuming(bool active);
    void slotScreenBrightnessChanged();
    void onKeyboardBrightnessChanged(int);

    void onPropertiesChanged(const QString &ifaceName, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void onDevicePropertiesChanged(const QString &ifaceName, const QVariantMap &changedProps, const QStringList &invalidatedProps);

private:
    // upower devices
    QMap<QString, OrgFreedesktopUPowerDeviceInterface *> m_devices;

    // brightness
    QMap<BrightnessControlType, float> m_cachedBrightnessMap;
    XRandrBrightness         *m_brightnessControl;
    XRandRX11Helper *m_randrHelper;
#ifdef HAVE_XF86VMODE
    XF86VModeGamma *m_gammaControl;
#endif

    OrgFreedesktopUPowerInterface *m_upowerInterface;
    OrgFreedesktopUPowerKbdBacklightInterface *m_kbdBacklight;
    int m_kbdMaxBrightness;

    // login1 interface
    QWeakPointer<QDBusInterface> m_login1Interface;
    // consolekit interface
    QWeakPointer<QDBusInterface> m_consolekitInterface;

    // buttons
    bool m_lidIsPresent;
    bool m_lidIsClosed;
    bool m_onBattery;
};

#endif // POWERDEVILUPOWERBACKEND_H
