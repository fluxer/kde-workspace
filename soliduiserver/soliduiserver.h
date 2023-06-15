/* This file is part of the KDE Project
   Copyright (c) 2005 Jean-Remy Falleri <jr.falleri@laposte.net>
   Copyright (c) 2005-2007 Kevin Ottens <ervin@kde.org>

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

#ifndef SOLIDUISERVER_H
#define SOLIDUISERVER_H

#include <kdedmodule.h>

#include <QMap>

class DeviceActionsDialog;

class SolidUiServer : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.SolidUiServer")

public:
    SolidUiServer(QObject* parent, const QList<QVariant>&);
    virtual ~SolidUiServer();

public Q_SLOTS:
    Q_SCRIPTABLE void showActionsDialog(const QString &udi,
                                        const QStringList &desktopFiles);

    Q_SCRIPTABLE int mountDevice(const QString &device, const QString &mountpoint, bool readonly = false);
    Q_SCRIPTABLE int unmountDevice(const QString &mountpoint);

    Q_SCRIPTABLE int mountUdi(const QString &udi);
    Q_SCRIPTABLE int unmountUdi(const QString &udi);

private Q_SLOTS:
    void onActionDialogFinished();

private:
    QMap<QString, DeviceActionsDialog*> m_udiToActionsDialog;
};
#endif
