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

#ifndef KFIREWALLCONFIG_H
#define KFIREWALLCONFIG_H

#include <kcmodule.h>

#include "ui_kfirewallconfig.h"

/**
 * Control look of KDE firewall
 *
 * @author Ivailo Monev (xakepa10@gmail.com)
 */
class KCMFirewall : public KCModule, public Ui_KFirewallDialog
{
    Q_OBJECT
public:
    // KCModule reimplementations
    KCMFirewall(QWidget* parent, const QVariantList&);
    ~KCMFirewall();

public Q_SLOTS:
    void load() final;
    void save() final;
    void defaults() final;

private:
    void addRuleRow();

private Q_SLOTS:
    void slotAddRule();
    void slotRemoveRule();

    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slotItemChanged(QTableWidgetItem* tablewidget);
    void slotTrafficChanged(const int value);
    void slotPortChanged(const int value);
    void slotActionChanged(const int value);

private:
    QString m_kfirewallconfigpath;
};

#endif // KFIREWALLCONFIG_H
