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

#ifndef KSCREENSAVER_H
#define KSCREENSAVER_H

#include <QObject>
#include <QProcess>
#include <QElapsedTimer>

class KScreenSaver : public QObject
{
    Q_OBJECT
public:
    KScreenSaver(QObject *parent = nullptr);
    ~KScreenSaver();

public Q_SLOTS:
    bool GetActive();
    uint GetActiveTime();
    uint GetSessionIdleTime();

    void Lock();
    bool SetActive(bool active);

    void SimulateUserActivity();

    uint Inhibit(const QString &application_name, const QString &reason_for_inhibit);
    uint Throttle(const QString &application_name, const QString &reason_for_inhibit);
    void UnInhibit(uint cookie);
    void UnThrottle(uint cookie);

Q_SIGNALS:
    void ActiveChanged(bool active);

private Q_SLOTS:
    void slotXScreenSaverOutput();
    void slotXScreenSaverError();

private:
    QProcess* m_xscreensaver;
    QElapsedTimer m_activetimer;
    uint m_inhibitionscounter;
    QList<uint> m_inhibitions;
};

#endif // KSCREENSAVER_H
