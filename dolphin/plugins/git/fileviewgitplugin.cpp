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

#include "fileviewgitplugin.h"
#include "gitcommitdialog.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaction.h>
#include <kicon.h>
#include <klocale.h>
#include <kdirnotify.h>
#include <kdebug.h>
#include <QDir>

#include <git2/errors.h>
#include <git2/global.h>
#include <git2/status.h>
#include <git2/index.h>
#include <git2/signature.h>
#include <git2/tree.h>
#include <git2/revparse.h>
#include <git2/commit.h>

K_PLUGIN_FACTORY(FileViewGitPluginFactory, registerPlugin<FileViewGitPlugin>();)
K_EXPORT_PLUGIN(FileViewGitPluginFactory("fileviewgitplugin"))

static char* s_gitupdatestrings[1] = { (char*)"*\0" }; // stack corruption? nah..

struct GitStatusPayload
{
    QStringList *result;
    QByteArray gitdirectory;
};

// path passed to git_status_file() has to be relative to the main git directory
static QByteArray getGitFile(const KFileItem &item, const QByteArray &gitdir)
{
    const QByteArray result = QFile::encodeName(item.localPath());
    return result.mid(gitdir.size(), result.size() - gitdir.size());
}

static QString getCompleteGitFile(const char* gitfile, const QByteArray &gitdir)
{
    QString result = QFile::decodeName(gitdir);
    if (!result.endsWith(QDir::separator())) {
        result.append(QDir::separator());
    }
    result.append(QFile::decodeName(gitfile));
    return result;
}

FileViewGitPlugin::FileViewGitPlugin(QObject *parent, const QList<QVariant> &args)
    : KVersionControlPlugin(parent),
    m_gitrepo(nullptr),
    m_addaction(nullptr),
    m_removeaction(nullptr),
    m_commitaction(nullptr)
{
    Q_UNUSED(args);

    git_libgit2_init();

    m_addaction = new KAction(this);
    m_addaction->setIcon(KIcon("svn-commit"));
    m_addaction->setText(i18nc("@action:inmenu", "<application>Git</application> Add"));
    connect(
        m_addaction, SIGNAL(triggered()),
        this, SLOT(slotAdd())
    );

    m_removeaction = new KAction(this);
    m_removeaction->setIcon(KIcon("list-remove"));
    m_removeaction->setText(i18nc("@action:inmenu", "<application>Git</application> Remove"));
    connect(
        m_removeaction, SIGNAL(triggered()),
        this, SLOT(slotRemove())
    );

    m_commitaction = new KAction(this);
    m_commitaction->setIcon(KIcon("svn-commit"));
    m_commitaction->setText(i18nc("@action:inmenu", "<application>Git</application> Commit..."));
    connect(
        m_commitaction, SIGNAL(triggered()),
        this, SLOT(slotCommit())
    );
}

FileViewGitPlugin::~FileViewGitPlugin()
{
    if (m_gitrepo) {
        kDebug() << "Done with" << m_directory;
        git_repository_free(m_gitrepo);
        m_gitrepo = nullptr;
    }

    git_libgit2_shutdown();
}

QString FileViewGitPlugin::fileName() const
{
    return QString::fromLatin1(".git");
}

bool FileViewGitPlugin::beginRetrieval(const QString &directory)
{
    if (m_gitrepo) {
        kDebug() << "Done with" << m_directory;
        git_repository_free(m_gitrepo);
        m_gitrepo = nullptr;
    }
    m_directory.clear();
    const QByteArray directorybytes = QFile::encodeName(directory);
    // NOTE: git_repository_open_ext() will look for .git in parent directories
    const int gitresult = git_repository_open_ext(&m_gitrepo, directorybytes.constData(), 0 , "/");
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not open" << directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return false;
    }
    m_directory = git_repository_workdir(m_gitrepo);
    kDebug() << "Initialized" << directory;
    return true;
}

void FileViewGitPlugin::endRetrieval()
{
}

KVersionControlPlugin::ItemVersion FileViewGitPlugin::itemVersion(const KFileItem &item) const
{
    if (!m_gitrepo) {
        kWarning() << "Not initialized" << m_directory;
        return KVersionControlPlugin::UnversionedVersion;
    }
    if (item.isDir()) {
        // git_status_file() cannot get the status of directories
        return KVersionControlPlugin::NormalVersion;
    }
    const QByteArray gitfile = getGitFile(item, m_directory);
    unsigned int gitstatusflags = 0;
    const int gitresult = git_status_file(&gitstatusflags, m_gitrepo, gitfile.constData());
    if (gitresult != GIT_OK) {
        kWarning() << "Could not get status" << gitfile << FileViewGitPlugin::getGitError();
        return KVersionControlPlugin::UnversionedVersion;
    }
    if (gitstatusflags & GIT_STATUS_INDEX_NEW || gitstatusflags & GIT_STATUS_WT_NEW) {
        return KVersionControlPlugin::AddedVersion;
    }
    if (gitstatusflags & GIT_STATUS_INDEX_MODIFIED) {
        return KVersionControlPlugin::LocallyModifiedVersion;
    } else if (gitstatusflags & GIT_STATUS_WT_MODIFIED) {
        return KVersionControlPlugin::LocallyModifiedUnstagedVersion;
    }
    if (gitstatusflags & GIT_STATUS_INDEX_DELETED || gitstatusflags & GIT_STATUS_WT_DELETED) {
        return KVersionControlPlugin::RemovedVersion;
    }
    if (gitstatusflags & GIT_STATUS_IGNORED) {
        return KVersionControlPlugin::IgnoredVersion;
    }
    if (gitstatusflags & GIT_STATUS_CONFLICTED) {
        return KVersionControlPlugin::ConflictingVersion;
    }
    return KVersionControlPlugin::NormalVersion;
}

QList<QAction*> FileViewGitPlugin::actions(const KFileItemList &items) const
{
    QList<QAction*> result;
    m_actionitems.clear();
    if (!m_gitrepo) {
        kWarning() << "Not initialized" << m_directory;
        return result;
    } else if (items.isEmpty()) {
        kDebug() << "No items" << m_directory;
        return result;
    }
    bool hasdir = false;
    foreach (const KFileItem &item, items) {
        if (item.isDir()) {
            m_actionitems.clear();
            m_actionitems.append(item);
            hasdir = true;
            break;
        } else {
            m_actionitems.append(item);
        }
    }
    if (hasdir) {
        const QStringList changedgitfiles = changedGitFiles();
        if (!changedgitfiles.isEmpty()) {
            result.append(m_commitaction);
        }
    } else {
        result.append(m_addaction);
        result.append(m_removeaction);
    }
    return result;
}

QStringList FileViewGitPlugin::changedGitFiles() const
{
    QStringList result;
    if (!m_gitrepo) {
        kWarning() << "Not initialized" << m_directory;
        return result;
    }
    git_status_options gitstatusoptions;
    int gitresult = git_status_options_init(&gitstatusoptions, GIT_STATUS_OPTIONS_VERSION);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not initialize status options" << m_directory << giterror;
        return result;
    }
    gitstatusoptions.flags = GIT_STATUS_OPT_UPDATE_INDEX;

    GitStatusPayload gitstatuspayload;
    gitstatuspayload.result = &result;
    gitstatuspayload.gitdirectory = m_directory;
    // NOTE: the callback is called only for paths the status of which has changed
    gitresult = git_status_foreach_ext(m_gitrepo, &gitstatusoptions, FileViewGitPlugin::gitStatusCallback, &gitstatuspayload);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get repository status" << m_directory << giterror;
        return result;
    }
    // qDebug() << Q_FUNC_INFO << m_directory << result;
    return result;
}

int FileViewGitPlugin::gitStatusCallback(const char *path, unsigned int status_flags, void *payload)
{
    GitStatusPayload* gitstatuspayload = static_cast<GitStatusPayload*>(payload);
    // qDebug() << Q_FUNC_INFO << path << status_flags;
    // check flags disregarding the ignored and conflicting files
    if (status_flags & GIT_STATUS_INDEX_NEW || status_flags & GIT_STATUS_WT_NEW) {
        gitstatuspayload->result->append(getCompleteGitFile(path, gitstatuspayload->gitdirectory));
    } else if (status_flags & GIT_STATUS_INDEX_MODIFIED || status_flags & GIT_STATUS_WT_MODIFIED) {
        gitstatuspayload->result->append(getCompleteGitFile(path, gitstatuspayload->gitdirectory));
    } else if (status_flags & GIT_STATUS_INDEX_DELETED || status_flags & GIT_STATUS_WT_DELETED) {
        gitstatuspayload->result->append(getCompleteGitFile(path, gitstatuspayload->gitdirectory));
    }
    return GIT_OK;
}

QByteArray FileViewGitPlugin::getGitError()
{
    const git_error* giterror = git_error_last();
    if (!giterror) {
        return QByteArray();
    }
    return QByteArray(giterror->message);
}

void FileViewGitPlugin::slotAdd()
{
    Q_ASSERT(!m_actionitems.isEmpty());
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    foreach (const KFileItem &item, m_actionitems) {
        const QByteArray gitfile = getGitFile(item, m_directory);
        emit infoMessage(i18n("Adding: %1", QFile::decodeName(gitfile)));
        gitresult = git_index_add_bypath(gitindex, gitfile.constData());
        if (gitresult != GIT_OK) {
            const QByteArray giterror = FileViewGitPlugin::getGitError();
            kWarning() << "Could not add path to repository" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            git_index_free(gitindex);
            return;
        }
    }

    emit infoMessage(i18n("Writing changes"));
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not write changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    kDebug() << "Done adding to" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_index_free(gitindex);

    // notify KDirLister about changes (to refresh the icons)
    QStringList changedgitfiles;
    changedgitfiles.reserve(m_actionitems.size());
    foreach (const KFileItem &item, m_actionitems) {
        changedgitfiles.append(item.localPath());
    }
    org::kde::KDirNotify::emitFilesChanged(changedgitfiles);
}

void FileViewGitPlugin::slotRemove()
{
    Q_ASSERT(!m_actionitems.isEmpty());
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    foreach (const KFileItem &item, m_actionitems) {
        const QByteArray gitfile = getGitFile(item, m_directory);
        emit infoMessage(i18n("Removing: %1", QFile::decodeName(gitfile)));
        gitresult = git_index_remove_bypath(gitindex, gitfile.constData());
        if (gitresult != GIT_OK) {
            const QByteArray giterror = FileViewGitPlugin::getGitError();
            kWarning() << "Could not remove path from repository" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            git_index_free(gitindex);
            return;
        }
    }

    emit infoMessage(i18n("Writing changes"));
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not write index changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    kDebug() << "Done removing from" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_index_free(gitindex);

    QStringList changedgitfiles;
    changedgitfiles.reserve(m_actionitems.size());
    foreach (const KFileItem &item, m_actionitems) {
        changedgitfiles.append(item.localPath());
    }
    org::kde::KDirNotify::emitFilesChanged(changedgitfiles);
}

void FileViewGitPlugin::slotCommit()
{
    emit infoMessage(i18n("Determening changed files"));
    const QStringList changedgitfiles = changedGitFiles();
    Q_ASSERT(!changedgitfiles.isEmpty());

    GitCommitDialog gitdialog(changedgitfiles, nullptr);
    if (gitdialog.exec() != QDialog::Accepted) {
        return;
    }
    const QByteArray gitmessage = gitdialog.message();

    Q_ASSERT(m_gitrepo != nullptr);
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    emit infoMessage(i18n("Updating index"));
    git_strarray gitupdatearray;
    gitupdatearray.strings = s_gitupdatestrings;
    gitupdatearray.count = 1;
    gitresult = git_index_update_all(gitindex, &gitupdatearray, NULL, NULL);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not update index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Getting repository head"));
    git_reference* gitreference = nullptr;
    gitresult = git_repository_head(&gitreference, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get repository head" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }
    const QByteArray gitspec = git_reference_name(gitreference);
    git_reference_free(gitreference);

    emit infoMessage(i18n("Parsing revision"));
    git_object *gitparent = nullptr;
    gitresult = git_revparse_single(&gitparent, m_gitrepo, gitspec.constData());
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not parse revision" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Writing changes"));
    git_oid gittreeobjectid;
    gitresult = git_index_write_tree(&gittreeobjectid, gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not write tree changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not write index changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Looking for the tree"));
    git_tree *gittree = nullptr;
    gitresult = git_tree_lookup(&gittree, m_gitrepo, &gittreeobjectid);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not lookup tree" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Getting default signature"));
    git_signature* gitsignature = nullptr;
    gitresult = git_signature_default(&gitsignature, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not get signature" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_tree_free(gittree);
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Commiting changes"));
    git_oid gitcommitobjectid;
    gitresult = git_commit_create_v(
        &gitcommitobjectid, m_gitrepo, gitspec.constData(),
        gitsignature, gitsignature,
        "UTF-8", gitmessage.constData(),
        gittree,
        gitparent ? 1 : 0, gitparent
    );
    if (gitresult != GIT_OK) {
        const QByteArray giterror = FileViewGitPlugin::getGitError();
        kWarning() << "Could not commit" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_signature_free(gitsignature);
        git_tree_free(gittree);
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }

    kDebug() << "Done commiting" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_signature_free(gitsignature);
    git_tree_free(gittree);
    git_object_free(gitparent);
    git_index_free(gitindex);

    org::kde::KDirNotify::emitFilesChanged(changedgitfiles);
}

#include "moc_fileviewgitplugin.cpp"
