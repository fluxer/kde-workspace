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
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kpasswdstore.h>
#include <kpassworddialog.h>
#include <kicon.h>
#include <klocale.h>
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
#include <git2/remote.h>

K_PLUGIN_FACTORY(FileViewGitPluginFactory, registerPlugin<FileViewGitPlugin>();)
K_EXPORT_PLUGIN(FileViewGitPluginFactory("fileviewgitplugin"))

static QByteArray getGitError()
{
    const git_error* giterror = git_error_last();
    if (!giterror) {
        return QByteArray();
    }
    return QByteArray(giterror->message);
}

// path passed to git_status_file() has to be relative to the main git directory
static QByteArray getGitFile(const KFileItem &item, const QByteArray &gitdir)
{
    const QByteArray result = QFile::encodeName(item.localPath());
    return result.mid(gitdir.size(), result.size() - gitdir.size());
}

static QByteArray getSSHKey()
{
    static const QString homepath = QDir::homePath();
    static const QStringList possiblekeys = QStringList()
        << QString::fromLatin1("id_ed25519")
        << QString::fromLatin1("id_rsa");
    foreach (const QString &key, possiblekeys) {
        const QString fullpath = homepath + QLatin1String("/.ssh/") + key;
        if (QFile::exists(fullpath)) {
            return QFile::encodeName(fullpath);
        }
    }
    const QString manualkey = KFileDialog::getOpenFileName(
        KUrl("kfiledialog:///FileViewGitPlugin"),
        QString(),
        nullptr,
        i18n("Select SSH key")
    );
    return QFile::encodeName(manualkey);
}

FileViewGitPlugin::FileViewGitPlugin(QObject *parent, const QList<QVariant> &args)
    : KVersionControlPlugin(parent),
    m_gitrepo(nullptr),
    m_addaction(nullptr),
    m_removeaction(nullptr),
    m_commitaction(nullptr),
    m_pushaction(nullptr),
    m_pullaction(nullptr)
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

    m_pushaction = new KAction(this);
    m_pushaction->setIcon(KIcon("go-top"));
    m_pushaction->setText(i18nc("@action:inmenu", "<application>Git</application> Push..."));
    connect(
        m_pushaction, SIGNAL(triggered()),
        this, SLOT(slotPush())
    );

    m_pullaction = new KAction(this);
    m_pullaction->setIcon(KIcon("go-bottom"));
    m_pullaction->setText(i18nc("@action:inmenu", "<application>Git</application> Pull..."));
    connect(
        m_pullaction, SIGNAL(triggered()),
        this, SLOT(slotPull())
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
        const QByteArray giterror = getGitError();
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
        kWarning() << "Could not get status" << gitfile << getGitError();
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
        // TODO: commit should be conditional
        result.append(m_commitaction);
        result.append(m_pushaction);
        result.append(m_pullaction);
    } else {
        result.append(m_addaction);
        result.append(m_removeaction);
    }
    return result;
}

void FileViewGitPlugin::slotAdd()
{
    Q_ASSERT(!m_actionitems.isEmpty());
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    foreach (const KFileItem &item, m_actionitems) {
        const QByteArray gitfile = getGitFile(item, m_directory);
        emit infoMessage(i18n("Adding: %1", QFile::decodeName(gitfile)));
        gitresult = git_index_add_bypath(gitindex, gitfile.constData());
        if (gitresult != GIT_OK) {
            const QByteArray giterror = getGitError();
            kWarning() << "Could not add path to repository" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            git_index_free(gitindex);
            return;
        }
    }

    emit infoMessage(i18n("Writing changes"));
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not write changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    kDebug() << "Done adding to" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_index_free(gitindex);
}

void FileViewGitPlugin::slotRemove()
{
    Q_ASSERT(!m_actionitems.isEmpty());
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    foreach (const KFileItem &item, m_actionitems) {
        const QByteArray gitfile = getGitFile(item, m_directory);
        emit infoMessage(i18n("Removing: %1", QFile::decodeName(gitfile)));
        gitresult = git_index_remove_bypath(gitindex, gitfile.constData());
        if (gitresult != GIT_OK) {
            const QByteArray giterror = getGitError();
            kWarning() << "Could not remove path from repository" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            git_index_free(gitindex);
            return;
        }
    }

    emit infoMessage(i18n("Writing changes"));
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not write index changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    kDebug() << "Done removing from" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_index_free(gitindex);
}

void FileViewGitPlugin::slotCommit()
{
    GitCommitDialog gitdialog;
    if (gitdialog.exec() != QDialog::Accepted) {
        return;
    }

    static const char* gitspec = "HEAD"; // TODO: option for it, detached HEAD?
    const QByteArray gitmessage = gitdialog.message();

    Q_ASSERT(m_gitrepo != nullptr);
    git_index* gitindex = nullptr;
    int gitresult = git_repository_index(&gitindex, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not get repository index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    emit infoMessage(i18n("Updating index"));
    static char* s_gitupdatestrings[1] = { (char*)"*\0" }; // stack corruption? nah..
    git_strarray gitupdatearray;
    gitupdatearray.strings = s_gitupdatestrings;
    gitupdatearray.count = 1;
    gitresult = git_index_update_all(gitindex, &gitupdatearray, NULL, NULL);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not update index" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Parsing revision"));
    git_object *gitparent = nullptr;
    gitresult = git_revparse_single(&gitparent, m_gitrepo, gitspec);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not parse revision" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_index_free(gitindex);
        return;
    }

    emit infoMessage(i18n("Writing changes"));
    git_oid gittreeobjectid;
    gitresult = git_index_write_tree(&gittreeobjectid, gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not write tree changes to repository" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        git_object_free(gitparent);
        git_index_free(gitindex);
        return;
    }
    gitresult = git_index_write(gitindex);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
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
        const QByteArray giterror = getGitError();
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
        const QByteArray giterror = getGitError();
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
        &gitcommitobjectid, m_gitrepo, gitspec,
        gitsignature, gitsignature,
        "UTF-8", gitmessage.constData(),
        gittree,
        gitparent ? 1 : 0, gitparent
    );
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
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
}

void FileViewGitPlugin::slotPush()
{
    emit infoMessage(i18n("Setting up push options"));
    git_push_options gitpushoptions;
    int gitresult = git_push_options_init(&gitpushoptions, GIT_PUSH_OPTIONS_VERSION);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not initialize push options" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }
    gitpushoptions.callbacks.certificate_check = FileViewGitPlugin::gitCertificateCallback;
    gitpushoptions.callbacks.credentials = FileViewGitPlugin::gitCredentialCallback;
    gitpushoptions.callbacks.payload = this;
    gitpushoptions.follow_redirects = GIT_REMOTE_REDIRECT_ALL;
    // NOTE: proxy settings may be taken from the environment
    gitpushoptions.proxy_opts.certificate_check = FileViewGitPlugin::gitCertificateCallback;
    gitpushoptions.proxy_opts.credentials = FileViewGitPlugin::gitCredentialCallback;

    emit infoMessage(i18n("Listing remotes"));
    git_strarray gitremotes;
    gitresult = git_remote_list(&gitremotes, m_gitrepo);
    if (gitresult != GIT_OK) {
        const QByteArray giterror = getGitError();
        kWarning() << "Could not list remotes" << m_directory << giterror;
        emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
        return;
    }

    emit infoMessage(i18n("Pushing remotes"));
    for (int i = 0; i < gitremotes.count; i++) {
        git_remote* gitremote = nullptr;
        gitresult = git_remote_lookup(&gitremote, m_gitrepo, gitremotes.strings[i]);
        if (gitresult != GIT_OK) {
            const QByteArray giterror = getGitError();
            kWarning() << "Could not lookup remote" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            return;
        }

        gitresult = git_remote_push(gitremote, NULL, &gitpushoptions);
        if (gitresult != GIT_OK) {
            const QByteArray giterror = getGitError();
            kWarning() << "Could not push remote" << m_directory << giterror;
            emit errorMessage(QString::fromLocal8Bit(giterror.constData(), giterror.size()));
            git_remote_free(gitremote);
            git_strarray_dispose(&gitremotes);
            return;
        }
        git_remote_free(gitremote);
    }

    kDebug() << "Done pushing" << m_directory << m_actionitems;
    emit operationCompletedMessage(i18n("Done"));
    git_strarray_dispose(&gitremotes);
}

void FileViewGitPlugin::slotPull()
{
    emit errorMessage(QString::fromLatin1("Not implemented"));
}

int FileViewGitPlugin::gitCertificateCallback(git_cert *cert, int valid, const char *host, void *payload)
{
    Q_UNUSED(cert);
    Q_UNUSED(payload);
    // qDebug() << Q_FUNC_INFO << valid;
    if (valid) {
        // not asking for valid certs
        return GIT_OK;
    }
    const int kmessageresult = KMessageBox::warningYesNo(
        nullptr,
        i18n("Certificate for %1 is not valid, continue anyway?", QString::fromUtf8(host))
    );
    if (kmessageresult == KMessageBox::Yes) {
        return GIT_OK;
    }
    return GIT_ERROR;
}

int FileViewGitPlugin::gitCredentialCallback(git_credential **out,
                                             const char *url, const char *username_from_url, unsigned int allowed_types,
                                             void *payload)
{
    // qDebug() << Q_FUNC_INFO << allowed_types;

    QByteArray gitsshkey;
    if (allowed_types & GIT_CREDENTIAL_SSH_KEY) {
        gitsshkey = getSSHKey();
        if (gitsshkey.isEmpty()) {
            kWarning() << "No SSH key found";
            return GIT_ERROR;
        }
    }

    FileViewGitPlugin* fileviewgitplugin = static_cast<FileViewGitPlugin*>(payload);
    KPasswdStore kpasswdstore(fileviewgitplugin);
    kpasswdstore.setStoreID(QString::fromLatin1("FileViewGitPlugin"));
    QByteArray kpasswdstorekey;
    const QString kpassworddialogcomment = QString::fromUtf8(url);
    const QString kpassworddialoguser = QString::fromUtf8(username_from_url);
    QString kpassworddialogpass;
    KPasswordDialog::KPasswordDialogFlags kpassworddialogflags = KPasswordDialog::ShowUsernameLine;
    if (!kpassworddialoguser.isEmpty()) {
        kpassworddialogflags |= KPasswordDialog::UsernameReadOnly;
    }
    if (!kpassworddialogcomment.isEmpty()) {
        kpassworddialogflags |= KPasswordDialog::ShowKeepPassword;
        kpasswdstorekey = KPasswdStore::makeKey(kpassworddialogcomment);
        kpassworddialogpass = kpasswdstore.getPasswd(kpasswdstorekey);
    }
    if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT || allowed_types & GIT_CREDENTIAL_SSH_KEY) {
        KPasswordDialog kpassworddialog(nullptr, kpassworddialogflags);
        kpassworddialog.addCommentLine(i18n("URL:"), kpassworddialogcomment);
        kpassworddialog.setUsername(kpassworddialoguser);
        kpassworddialog.setPassword(kpassworddialogpass);
        if (!kpassworddialogpass.isEmpty()) {
            kpassworddialog.setKeepPassword(true);
        }
        if (kpassworddialog.exec() != QDialog::Accepted) {
            return 1;
        }
        const QByteArray gituser = kpassworddialog.username().toUtf8();
        const QByteArray gitpass = kpassworddialog.password().toUtf8();
        if (!kpasswdstorekey.isEmpty() && !gitpass.isEmpty()) {
            kpasswdstore.storePasswd(kpasswdstorekey, kpassworddialog.password());
        }
        if (allowed_types & GIT_CREDENTIAL_SSH_KEY) {
            QByteArray gitsshkeypub = gitsshkey;
            gitsshkeypub.append(".pub");
            return git_credential_ssh_key_new(
                out,
                gituser.constData(),
                gitsshkeypub.constData(), gitsshkey.constData(),
                gitpass.constData()
            );
        }
        return git_credential_userpass_plaintext_new(
            out,
            gituser.constData(), gitpass.constData()
        );
    }
    if (allowed_types & GIT_CREDENTIAL_USERNAME) {
        KPasswordDialog kpassworddialog(nullptr, kpassworddialogflags);
        kpassworddialog.addCommentLine(i18n("URL:"), kpassworddialogcomment);
        kpassworddialog.setUsername(kpassworddialoguser);
        if (kpassworddialog.exec() != QDialog::Accepted) {
            return 1;
        }
        const QByteArray gituser = kpassworddialog.username().toUtf8();
        return git_credential_username_new(out, gituser.constData());
    }
    return 1;
}

#include "moc_fileviewgitplugin.cpp"
