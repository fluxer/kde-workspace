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

#include "keyboardoptionsdialog.h"

#include <QHeaderView>
#include <klocale.h>
#include <kkeyboardlayout.h>
#include <kdebug.h>

static const char s_optionsseparator = ',';
static const char s_optionsgroupseparator = ':';

KCMKeyboardOptionsDialog::KCMKeyboardOptionsDialog(QWidget *parent)
    : KDialog(parent),
    m_widget(nullptr),
    m_layout(nullptr),
    m_optionstree(nullptr)
{
    setCaption(i18n("Keyboard Options"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_enabledi18n = i18n("Enabled");
    m_disabledi18n = i18n("Disabled");

    m_widget = new QWidget(this);
    m_layout = new QGridLayout(m_widget);

    m_optionstree = new QTreeWidget(m_widget);
    m_optionstree->setColumnCount(2);
    QStringList treeheaders = QStringList()
        << i18n("Option")
        << i18n("State");
    m_optionstree->setHeaderLabels(treeheaders);
    m_optionstree->setRootIsDecorated(false);
    m_optionstree->header()->setStretchLastSection(false);
    m_optionstree->header()->setResizeMode(0, QHeaderView::Stretch);
    m_optionstree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    connect(
        m_optionstree, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
        this, SLOT(slotItemChanged(QTreeWidgetItem*,int))
    );
    m_layout->addWidget(m_optionstree, 0, 0);

    setMainWidget(m_widget);

    adjustSize();
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardOptionsDialog");
    restoreDialogSize(kconfiggroup);
}

KCMKeyboardOptionsDialog::~KCMKeyboardOptionsDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardOptionsDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

QByteArray KCMKeyboardOptionsDialog::options() const
{
    QByteArray result;
    foreach (const QByteArray &option, m_options) {
        if (!result.isEmpty()) {
            result.append(s_optionsseparator);
        }
        result.append(option);
    }
    return result;
}

void KCMKeyboardOptionsDialog::setOptions(const QByteArray &options)
{
    m_options = options.split(s_optionsseparator);

    m_optionstree->clear();
    foreach (const QByteArray &option, KKeyboardLayout::optionNames()) {
        if (!option.contains(s_optionsgroupseparator)) {
            continue;
        }
        QTreeWidgetItem* optionitem = new QTreeWidgetItem();
        optionitem->setData(0, Qt::UserRole, option);
        optionitem->setText(0, KKeyboardLayout::optionDescription(option));
        const bool isoptionenabled = m_options.contains(option);
        optionitem->setText(1, isoptionenabled ? m_enabledi18n : m_disabledi18n);
        optionitem->setCheckState(1, isoptionenabled ? Qt::Checked : Qt::Unchecked);
        m_optionstree->addTopLevelItem(optionitem);
    }
}

void KCMKeyboardOptionsDialog::slotItemChanged(QTreeWidgetItem *optionitem, const int column)
{
    Q_UNUSED(column);
    const bool enableoption = (optionitem->checkState(1) == Qt::Checked);
    optionitem->setText(1, enableoption ? m_enabledi18n : m_disabledi18n);
    const QByteArray option = optionitem->data(0, Qt::UserRole).toByteArray();
    if (enableoption) {
        m_options.append(option);
    } else {
        m_options.removeAll(option);
    }
}

#include "moc_keyboardoptionsdialog.cpp"
