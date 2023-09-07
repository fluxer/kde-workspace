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

#include <klocale.h>
#include <kdebug.h>

KCMKeyboardOptionsDialog::KCMKeyboardOptionsDialog(QWidget *parent)
    : KDialog(parent),
    m_widget(nullptr),
    m_layout(nullptr)
{
    setCaption(i18n("Keyboard Options"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_widget = new QWidget(this);
    m_layout = new QGridLayout(m_widget);

    setMainWidget(m_widget);

    // TODO:

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
    return m_options;
}

void KCMKeyboardOptionsDialog::setOptions(const QByteArray &options)
{
    m_options = options;
}

#include "moc_keyboardoptionsdialog.cpp"
