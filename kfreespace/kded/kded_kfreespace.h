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

#ifndef KFREESPACE_KDED_H
#define KFREESPACE_KDED_H

#include "kfreespaceimpl.h"

#include <QList>
#include <kdedmodule.h>
#include <kdirwatch.h>

class KFreeSpaceImpl;

class KFreeSpaceModule: public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kfreespace")

public:
    KFreeSpaceModule(QObject *parent, const QList<QVariant> &args);
    ~KFreeSpaceModule();

private Q_SLOTS:
    void slotInit();
    void slotDeviceAdded(const QString &udi);
    void slotDeviceRemoved(const QString &udi);
    void slotAccessibilityChanged(bool accessible, const QString &udi);

private:
    KDirWatch* m_dirwatch;
    QList<KFreeSpaceImpl*> m_freespaces;
};

#endif // KFREESPACE_KDED_H
