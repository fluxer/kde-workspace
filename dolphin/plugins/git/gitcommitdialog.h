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

#ifndef GITCOMMITDIALOG_H
#define GITCOMMITDIALOG_H

#include <kdialog.h>
#include <kvbox.h>
#include <ktextedit.h>
#include <ktabwidget.h>
#include <KTextEditor/EditorChooser>

class GitCommitDialog : public KDialog
{
    Q_OBJECT
public:
    GitCommitDialog(QWidget *parent = nullptr);
    ~GitCommitDialog();

    void setupWidgets(const QStringList &changedfiles, const QString &diff);
    QByteArray message() const;

private:
    KVBox* m_mainvbox;
    KTextEdit* m_commit;
    KTabWidget* m_detailstab;
    KTextEdit* m_changedfiles;
    KTextEdit* m_difffiles;
    KTextEditor::Document* m_diffdocument;
};

#endif // GITCOMMITDIALOG_H
