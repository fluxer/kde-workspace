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

#ifndef DEVICENOTIFIER_H
#define DEVICENOTIFIER_H

#include <QCheckBox>
#include <QSpacerItem>
#include <KConfigDialog>
#include <Plasma/ScrollWidget>
#include <Plasma/PopupApplet>

class DeviceNotifierWidget;

class DeviceNotifier : public Plasma::PopupApplet
{
    Q_OBJECT
public:
    DeviceNotifier(QObject *parent, const QVariantList &args);
    ~DeviceNotifier();

    // Plasma::Applet reimplementation
    void init() final;
    // Plasma::PopupApplet reimplementation
    QGraphicsWidget* graphicsWidget() final;
    void createConfigurationInterface(KConfigDialog *parent) final;

protected:
    // Plasma::PopupApplet reimplementation
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const final;

private Q_SLOTS:
    void slotConfigAccepted();

private:
    friend DeviceNotifierWidget;
    Plasma::ScrollWidget *m_plasmascrollwidget;
    DeviceNotifierWidget *m_devicenotifierwidget;
    QCheckBox* m_removablebox;
    QSpacerItem* m_spacer;
};

K_EXPORT_PLASMA_APPLET(devicenotifier, DeviceNotifier)

#endif // DEVICENOTIFIER_H
