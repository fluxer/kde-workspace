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

#ifndef KFREESPACECONFIG_H
#define KFREESPACECONFIG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QList>
#include <kcmodule.h>

class KFreeSpaceBox;

/**
 * Control KDE free space notifier
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMFreeSpace : public KCModule
{
    Q_OBJECT
public:
    // KCModule reimplementations
    KCMFreeSpace(QWidget *parent, const QVariantList &args);
    ~KCMFreeSpace();

public Q_SLOTS:
    void load() final;
    void save() final;
    void defaults() final;

    void slotDeviceChanged();

private:
    QVBoxLayout* m_layout;
    QSpacerItem* m_spacer;
    QList<KFreeSpaceBox*> m_deviceboxes;
};

#endif // KFREESPACECONFIG_H
