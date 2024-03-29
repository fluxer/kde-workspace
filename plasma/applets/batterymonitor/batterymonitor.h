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

#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <Plasma/PopupApplet>

class BatteryMonitorWidget;

class BatteryMonitor : public Plasma::PopupApplet
{
    Q_OBJECT
public:
    BatteryMonitor(QObject *parent, const QVariantList &args);
    ~BatteryMonitor();

    // Plasma::Applet reimplementation
    void init() final;
    // Plasma::PopupApplet reimplementation
    QGraphicsWidget* graphicsWidget() final;

    // Plasma::Applet reimplementations
public Q_SLOTS:
    void configChanged();
protected:
    void saveState(KConfigGroup &group) const final;

private:
    friend BatteryMonitorWidget;
    BatteryMonitorWidget *m_batterywidget;
};

K_EXPORT_PLASMA_APPLET(battery, BatteryMonitor)

#endif // BATTERYMONITOR_H
