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

#ifndef KDEMETAINFO_H
#define KDEMETAINFO_H

#include <kcmodule.h>

#include "ui_kmetainfoconfig.h"

/**
 * Control KFileMetaInfo output of KDE applications
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMMetaInfo : public KCModule, public Ui_KMetaInfoDialog
{
    Q_OBJECT
public:
    KCMMetaInfo(QWidget* parent, const QVariantList&);
    ~KCMMetaInfo();

    // KCModule reimplementations
public Q_SLOTS:
    void load() final;
    void save() final;

private Q_SLOTS:
    void slotPluginItemChanged(QTreeWidgetItem *item, int column);
    void slotMetaItemChanged(QListWidgetItem *item);

private:
    void loadMetaInfo();
};

#endif // KDEMETAINFO_H
