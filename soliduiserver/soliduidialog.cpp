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

#include "soliduidialog.h"
#include "soliduiserver_common.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfiggroup.h>

SolidUiDialog::SolidUiDialog(const Solid::Device &soliddevice,
                             const QList<KServiceAction> &kserviceactions,
                             QWidget *parent)
    : KDialog(parent),
    m_soliddevice(soliddevice),
    m_serviceactions(kserviceactions),
    m_mainwidget(nullptr),
    m_mainlayout(nullptr),
    m_devicepixmap(nullptr),
    m_devicelabel(nullptr),
    m_listwidget(nullptr)
{
    Q_ASSERT(kserviceactions.size() > 0);
    const KIcon deviceicon = KIcon(m_soliddevice.icon());
    setWindowIcon(deviceicon);
    setWindowTitle(i18n("Actions for %1", m_soliddevice.description()));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    m_mainwidget = new QWidget(this);
    m_mainlayout = new QGridLayout(m_mainwidget);

    const int dialogiconsize = KIconLoader::global()->currentSize(KIconLoader::Dialog);
    m_devicepixmap = new KPixmapWidget(m_mainwidget);
    m_devicepixmap->setPixmap(deviceicon.pixmap(dialogiconsize));
    m_mainlayout->addWidget(m_devicepixmap, 0, 0);
    m_devicelabel = new QLabel(m_mainwidget);
    m_devicelabel->setText(m_soliddevice.description());
    m_devicelabel->setAlignment(Qt::AlignCenter);
    m_devicelabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_mainlayout->addWidget(m_devicelabel, 0, 1);

    m_listwidget = new KListWidget(m_mainwidget);
    int itemscounter = 0;
    foreach (const KServiceAction &kserviceaction, kserviceactions) {
        QListWidgetItem* listitem = new QListWidgetItem(m_listwidget);
        listitem->setText(kserviceaction.text());
        listitem->setIcon(KIcon(kserviceaction.icon()));
        listitem->setData(Qt::UserRole, itemscounter);
        m_listwidget->addItem(listitem);
        itemscounter++;
    }
    connect(
        m_listwidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotItemSelectionChanged())
    );
    m_mainlayout->addWidget(m_listwidget, 1, 0, 1, 2);
    setMainWidget(m_mainwidget);

    adjustSize();
    KConfigGroup kconfiggroup(KGlobal::config(), "SolidUiDialog");
    restoreDialogSize(kconfiggroup);

    enableButtonOk(false);
    connect(
        this, SIGNAL(okClicked()),
        this, SLOT(slotOkClicked())
    );
}

SolidUiDialog::~SolidUiDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "SolidUiDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

void SolidUiDialog::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> selecteditems = m_listwidget->selectedItems();
    if (selecteditems.size() == 0) {
        return;
    }
    enableButtonOk(true);
}

void SolidUiDialog::slotOkClicked()
{
    const QList<QListWidgetItem*> selecteditems = m_listwidget->selectedItems();
    if (selecteditems.size() == 0) {
        return;
    }
    const QListWidgetItem* selecteditem = selecteditems.first();
    const int kserviceactionindex = selecteditem->data(Qt::UserRole).toInt();
    Q_ASSERT(kserviceactionindex >= 0 && kserviceactionindex < m_serviceactions.size());
    kExecuteAction(m_serviceactions.at(kserviceactionindex), m_soliddevice.udi());
}

#include "moc_soliduidialog.cpp"
