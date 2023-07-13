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
#include "fileviewgitplugin.h"

#include <klocale.h>
#include <kdebug.h>

#include <git2/buffer.h>
#include <git2/message.h>
#include <git2/errors.h>

GitCommitDialog::GitCommitDialog(const QStringList &changedfiles, const QString &diff, QWidget *parent)
    : KDialog(parent),
    m_mainvbox(nullptr),
    m_commit(nullptr),
    m_detailstab(nullptr),
    m_changedfiles(nullptr),
    m_difffiles(nullptr)
{
    setCaption(i18nc("@title:window", "<application>Git</application> Commit"));
    setButtons(KDialog::Details| KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setButtonText(KDialog::Ok, i18nc("@action:button", "Commit"));

    m_mainvbox = new KVBox(this);
    setMainWidget(m_mainvbox);

    m_commit = new KTextEdit(m_mainvbox);
    m_commit->setLineWrapMode(QTextEdit::FixedColumnWidth);
    m_commit->setLineWrapColumnOrWidth(72);

    m_detailstab = new KTabWidget(m_mainvbox);
    m_changedfiles = new KTextEdit(m_detailstab);
    m_changedfiles->setReadOnly(true);
    m_changedfiles->setText(changedfiles.join(QLatin1String("\n")));
    m_detailstab->addTab(m_changedfiles, KIcon("folder-documents"), i18n("Staged files"));
    m_difffiles = new KTextEdit(m_detailstab);
    m_difffiles->setReadOnly(true);
    m_difffiles->setText(diff);
    // TODO: syntax highlighter for the diff
    m_detailstab->addTab(m_difffiles, KIcon("text-x-patch"), i18n("Staged changes"));
    setDetailsWidget(m_detailstab);

    KConfigGroup kconfiggroup(KGlobal::config(), "GitCommitDialog");
    restoreDialogSize(kconfiggroup);
}

GitCommitDialog::~GitCommitDialog()
{
    KConfigGroup kconfiggroup(KGlobal::config(), "GitCommitDialog");
    saveDialogSize(kconfiggroup);
    KGlobal::config()->sync();
}

QByteArray GitCommitDialog::message() const
{
    const QByteArray gitmessage = m_commit->toPlainText().toUtf8();
    git_buf gitbuffer = GIT_BUF_INIT;
    int gitresult = git_message_prettify(&gitbuffer, gitmessage.constData(), 1, '#');
    if (gitresult != GIT_OK) {
        kWarning() << "Could not prettify message" << gitmessage << FileViewGitPlugin::getGitError();
        return gitmessage;
    }
    const QByteArray gitprettymessage(gitbuffer.ptr, gitbuffer.size);
    git_buf_dispose(&gitbuffer);
    return gitprettymessage;
}

#include "moc_gitcommitdialog.cpp"
