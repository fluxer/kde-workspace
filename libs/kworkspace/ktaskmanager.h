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

#ifndef KTASKMANAGER_H
#define KTASKMANAGER_H

#include "kworkspace_export.h"

#include <QObject>
#include <qwindowdefs.h>

class KTaskManagerPrivate;

class KWORKSPACE_EXPORT KTaskManager : public QObject
{
    Q_OBJECT
public:
    KTaskManager(QObject *parent = nullptr);
    ~KTaskManager();

    QList<WId> tasks() const;
    static bool isActive(const WId task);
    static bool demandsAttention(const WId task);
    static void activateRaiseOrIconify(const WId task);

    static KTaskManager* self();

Q_SIGNALS:
    void taskAdded(const WId task);
    void taskChanged(const WId task);
    void taskRemoved(const WId task);

private:
    friend KTaskManagerPrivate;
    Q_DISABLE_COPY(KTaskManager);
    KTaskManagerPrivate* d;
    
    Q_PRIVATE_SLOT(d, void _k_slotNewWindow(const WId window));
    Q_PRIVATE_SLOT(d, void _k_slotChangedWindow(const WId window));
    Q_PRIVATE_SLOT(d, void _k_slotRemovedWindow(const WId window));
};

#endif // KTASKMANAGER_H
