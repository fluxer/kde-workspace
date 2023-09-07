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

#include "keyboardlayoutdialog.h"

#include <klocale.h>
#include <kdebug.h>

KCMKeyboardLayoutDialog::KCMKeyboardLayoutDialog(const QList<KKeyboardType> &filter, QWidget *parent)
    : KDialog(parent),
    m_filter(filter),
    m_keyboardtype(KKeyboardLayout::defaultLayout()),
    m_widget(nullptr),
    m_layout(nullptr),
    m_layoutslabel(nullptr),
    m_layoutsbox(nullptr),
    m_variantslabel(nullptr),
    m_variantsbox(nullptr)
{
    setCaption(i18n("Keyboard Layout"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_widget = new QWidget(this);
    m_layout = new QGridLayout(m_widget);

    m_layoutslabel = new QLabel(m_widget);
    m_layoutslabel->setText(i18n("Layout:"));
    m_layout->addWidget(m_layoutslabel, 0, 0);
    m_layoutsbox = new QComboBox(m_widget);
    m_layoutsbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    foreach (const QByteArray &layoutlayout, KKeyboardLayout::layoutNames()) {
        m_layoutsbox->addItem(KKeyboardLayout::layoutDescription(layoutlayout), layoutlayout);
    }
    connect(
        m_layoutsbox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotLayoutIndexChanged(int))
    );
    m_layout->addWidget(m_layoutsbox, 0, 1);

    m_variantslabel = new QLabel(m_widget);
    m_variantslabel->setText(i18n("Variant:"));
    m_layout->addWidget(m_variantslabel, 1, 0);
    m_variantsbox = new QComboBox(m_widget);
    m_variantsbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(
        m_variantsbox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotVariantIndexChanged(int))
    );
    m_layout->addWidget(m_variantsbox, 1, 1);

    setMainWidget(m_widget);

    adjustSize();
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardLayoutDialog");
    restoreDialogSize(kconfiggroup);
}

KCMKeyboardLayoutDialog::~KCMKeyboardLayoutDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "KCMKeyboardLayoutDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

KKeyboardType KCMKeyboardLayoutDialog::keyboardType() const
{
    return m_keyboardtype;
}

void KCMKeyboardLayoutDialog::setKeyboardType(const KKeyboardType &layout)
{
    m_keyboardtype = layout;
    m_variantsbox->blockSignals(true);
    const int layoutindex = m_layoutsbox->findData(m_keyboardtype.layout);
    if (layoutindex >= 0) {
        m_layoutsbox->setCurrentIndex(layoutindex);
    } else {
        kWarning() << "Could not find the keyboard layout" << m_keyboardtype.layout;
    }
    const int variantindex = m_variantsbox->findData(m_keyboardtype.variant);
    if (variantindex >= 0) {
        m_variantsbox->setCurrentIndex(variantindex);
    } else {
        kWarning() << "Could not find the keyboard variant" << m_keyboardtype.variant;
    }
    m_variantsbox->blockSignals(false);
}

bool KCMKeyboardLayoutDialog::filterLayout(const QByteArray &layout, const QByteArray &variant) const
{
    foreach (const KKeyboardType &filter, m_filter) {
        if (filter.layout == layout && filter.variant == variant) {
            return true;
        }
    }
    return false;
}

void KCMKeyboardLayoutDialog::slotLayoutIndexChanged(const int index)
{
    m_variantsbox->clear();
    const QByteArray layoutlayout = m_layoutsbox->itemData(index).toByteArray();
    // add "None"
    m_variantsbox->addItem(KKeyboardLayout::variantDescription(layoutlayout, QByteArray()), QByteArray());
    foreach (const QByteArray &layoutvariant, KKeyboardLayout::variantNames(layoutlayout)) {
        m_variantsbox->addItem(KKeyboardLayout::variantDescription(layoutlayout, layoutvariant), layoutvariant);
    }
    enableButtonOk(!filterLayout(layoutlayout, m_keyboardtype.variant));
    m_keyboardtype.layout = layoutlayout;
}

void KCMKeyboardLayoutDialog::slotVariantIndexChanged(const int index)
{
    const QByteArray layoutvariant = m_variantsbox->itemData(index).toByteArray();
    enableButtonOk(!filterLayout(m_keyboardtype.layout, layoutvariant));
    m_keyboardtype.variant = layoutvariant;
}

#include "moc_keyboardlayoutdialog.cpp"
