/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#ifndef LOCKOUT_H
#define LOCKOUT_H

#include <QGraphicsLinearLayout>
#include <QCheckBox>
#include <QSpacerItem>
#include <QDBusServiceWatcher>
#include <Plasma/Applet>
#include <Plasma/IconWidget>
#include <KConfigDialog>
#include <KMessageWidget>

class LockoutApplet : public Plasma::Applet
{
    Q_OBJECT
public:
    LockoutApplet(QObject *parent, const QVariantList &args);

    // Plasma::Applet reimplementations
    void init() final;
    void createConfigurationInterface(KConfigDialog *parent) final;

protected:
    // Plasma::Applet reimplementation
    void constraintsEvent(Plasma::Constraints constraints) final;

private Q_SLOTS:
    void slotUpdateButtons();
    void slotScreensaverRegistered(const QString &service);
    void slotScreensaverUnregistered(const QString &service);
    void slotLock();
    void slotSwitch();
    void slotShutdown();
    void slotToRam();
    void slotToDisk();
    void slotHybrid();
    void slotConfigAccepted();

private:
    void updateWidgets();
    void updateOrientation();

    QGraphicsLinearLayout* m_layout;
    Plasma::IconWidget* m_lockwidget;
    Plasma::IconWidget* m_switchwidget;
    Plasma::IconWidget* m_shutdownwidget;
    Plasma::IconWidget* m_toramwidget;
    Plasma::IconWidget* m_todiskwidget;
    Plasma::IconWidget* m_hybridwidget;

    bool m_showlock;
    bool m_showswitch;
    bool m_showshutdown;
    bool m_showtoram;
    bool m_showtodisk;
    bool m_showhybrid;

    KMessageWidget* m_buttonsmessage;
    QCheckBox* m_lockbox;
    QCheckBox* m_switchbox;
    QCheckBox* m_shutdownbox;
    QCheckBox* m_torambox;
    QCheckBox* m_todiskbox;
    QCheckBox* m_hybridbox;
    QSpacerItem* m_spacer;

    QDBusServiceWatcher* m_screensaverwatcher;
};

K_EXPORT_PLASMA_APPLET(lockout, LockoutApplet)

#endif // LOCKOUT_H
