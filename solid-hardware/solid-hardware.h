/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

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

#ifndef SOLID_HARDWARE_H
#define SOLID_HARDWARE_H

#include <QCoreApplication>
#include <QEventLoop>

#include <solid/storageaccess.h>
#include <solid/opticaldrive.h>

class SolidHardware : public QCoreApplication
{
    Q_OBJECT
public:
    SolidHardware(int &argc, char **argv) : QCoreApplication(argc, argv), m_error(0) {}

    static bool doIt();

    bool hwList(bool interfaces);
    bool hwCapabilities(const QString &udi);
    bool hwQuery(const QString &parentUdi, const QString &query);
    bool listen();

    enum VolumeCallType { Mount, Unmount, Eject };
    bool hwVolumeCall(VolumeCallType type, const QString &udi);

private:
    QEventLoop m_loop;
    int m_error;
    QString m_errorString;

private slots:
    void slotStorageResult(Solid::ErrorType error, const QString &errorData, const QString &udi);
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);
};

Q_DECLARE_METATYPE(QList<int>)

#endif // SOLID_HARDWARE_H
