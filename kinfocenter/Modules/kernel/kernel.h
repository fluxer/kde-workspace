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

#ifndef KCONTROL_KERNEL_H
#define KCONTROL_KERNEL_H

#include <QTreeWidget>
#include <kcmodule.h>

class KCMKernel : public KCModule
{
   Q_OBJECT
public:
    explicit KCMKernel(QWidget *parent = 0, const QVariantList &list = QVariantList());

public Q_SLOTS:
    void load();

private:
    QTreeWidget* m_treewidget;
};

#endif // KCONTROL_KERNEL_H
