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

#ifndef FILEVIEWGITPLUGIN_H
#define FILEVIEWGITPLUGIN_H

#include <kfileitem.h>
#include <kversioncontrolplugin.h>

#include <git2/repository.h>

/**
 * @brief Git implementation for the KVersionControlPlugin interface.
 */
class FileViewGitPlugin : public KVersionControlPlugin
{
    Q_OBJECT
public:
    FileViewGitPlugin(QObject *parent, const QList<QVariant> &args);
    ~FileViewGitPlugin();

    QString fileName() const final;
    bool beginRetrieval(const QString &directory) final;
    void endRetrieval() final;
    KVersionControlPlugin::ItemVersion itemVersion(const KFileItem& item) const final;
    QList<QAction*> actions(const KFileItemList &items) const final;

    QStringList changedGitFiles() const;

    static int gitStatusCallback(const char *path, unsigned int status_flags, void *payload);

    static QByteArray getGitError();

private Q_SLOTS:
    void slotAdd();
    void slotRemove();
    void slotCommit();

private:
    QByteArray m_directory;
    git_repository* m_gitrepo;
    QAction* m_addaction;
    QAction* m_removeaction;
    QAction* m_commitaction;
    mutable KFileItemList m_actionitems;
};

#endif // FILEVIEWGITPLUGIN_H

