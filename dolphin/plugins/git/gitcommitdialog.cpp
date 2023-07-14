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

#include <kglobalsettings.h>
#include <klocale.h>
#include <kdebug.h>
#include <KTextEditor/View>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Cursor>

#include <git2/buffer.h>
#include <git2/message.h>
#include <git2/errors.h>

GitCommitDialog::GitCommitDialog(QWidget *parent)
    : KDialog(parent),
    m_mainvbox(nullptr),
    m_commit(nullptr),
    m_detailstab(nullptr),
    m_changedfiles(nullptr),
    m_difffiles(nullptr),
    m_diffdocument(nullptr),
    m_commits(nullptr)
{
    setCaption(i18nc("@title:window", "<application>Git</application> Commit"));
    setButtons(KDialog::Details | KDialog::Ok | KDialog::Cancel);
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
    m_detailstab->addTab(m_changedfiles, KIcon("folder-documents"), i18n("Staged files"));
    m_diffdocument = KTextEditor::EditorChooser::editor()->createDocument(m_detailstab);
    if (m_diffdocument) {
        m_diffdocument->setHighlightingMode("Diff");
        KTextEditor::View* diffview = m_diffdocument->createView(m_detailstab);
        KTextEditor::ConfigInterface* diffconfig = qobject_cast<KTextEditor::ConfigInterface*>(diffview);
        if (diffconfig) {
            // line numbers will not represent the line number in the changed file, disable them
            diffconfig->setConfigValue("line-numbers", false);
        }
        m_detailstab->addTab((QWidget*)diffview, KIcon("text-x-patch"), i18n("Staged changes"));
    } else {
        kWarning() << "Could not create text editor, using fallback";
        m_difffiles = new KTextEdit(m_detailstab);
        m_difffiles->setReadOnly(true);
        m_detailstab->addTab(m_difffiles, KIcon("text-x-patch"), i18n("Staged changes"));
    }
    m_commits = new KTextEdit(m_detailstab);
    m_commits->setReadOnly(true);
    // fixed font for correct spacing
    m_commits->setFont(KGlobalSettings::fixedFont());
    m_detailstab->addTab(m_commits, KIcon ("text-x-changelog"), i18n("Commits"));
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

void GitCommitDialog::setupWidgets(const QStringList &changedfiles, const QString &diff, const QString &commits)
{
    m_changedfiles->setText(changedfiles.join(QLatin1String("\n")));
    if (m_diffdocument) {
        // by not re-setting the text the cursor position and selection are preserved
        if (m_diffdocument->text() != diff) {
            // NOTE: can't set the text in read-only mode
            m_diffdocument->setReadWrite(true);
            m_diffdocument->setText(diff);
            // NOTE: after KTextEditor::Document::setText() the cursor is at the end
            m_diffdocument->activeView()->setCursorPosition(KTextEditor::Cursor::start());
            m_diffdocument->setReadWrite(false);
        }
    } else {
        m_difffiles->setText(diff);
    }
    m_commits->setText(commits);
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
