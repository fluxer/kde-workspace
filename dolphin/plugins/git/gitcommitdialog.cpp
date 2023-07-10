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

#include "gitcommitdialog.h"

#include <klocale.h>

GitCommitDialog::GitCommitDialog(QWidget *parent)
    : KDialog(parent),
    m_vbox(nullptr),
    m_commit(nullptr)
{
    setCaption(i18nc("@title:window", "<application>Git</application> Commit"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setButtonText(KDialog::Ok, i18nc("@action:button", "Commit"));

    m_vbox = new KVBox(this);
    setMainWidget(m_vbox);

    m_commit = new KTextEdit(m_vbox);
}

GitCommitDialog::~GitCommitDialog()
{
}

QByteArray GitCommitDialog::message() const
{
    return m_commit->toPlainText().toUtf8();
}

#include "moc_gitcommitdialog.cpp"
