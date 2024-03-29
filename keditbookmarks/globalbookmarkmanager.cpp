/* This file is part of the KDE project
   Copyright (C) 2000, 2010 David Faure <faure@kde.org>
   Copyright (C) 2002-2003 Alexander Kellett <lypanov@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published by
   the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "globalbookmarkmanager.h"
#include <kglobal.h>
#include <klocale.h>
#include <QDateTime>
#include <kdebug.h>
#include "kbookmarkmanager.h"
#include "kbookmarkmodel/model.h"
#include "kbookmarkmodel/commandhistory.h"

GlobalBookmarkManager *GlobalBookmarkManager::s_mgr = 0;

GlobalBookmarkManager::GlobalBookmarkManager()
    : QObject(0),
      m_mgr(0), m_model(0)
{
}

GlobalBookmarkManager::~GlobalBookmarkManager()
{
}

KBookmarkGroup GlobalBookmarkManager::root()
{
    return mgr()->root();
}

KBookmark GlobalBookmarkManager::bookmarkAt(const QString &a)
{
    return self()->mgr()->findByAddress(a);
}

bool GlobalBookmarkManager::managerSave() { return mgr()->save(); }
void GlobalBookmarkManager::saveAs(const QString &fileName) { mgr()->saveAs(fileName); }
void GlobalBookmarkManager::setUpdate(bool update) { mgr()->setUpdate(update); }
QString GlobalBookmarkManager::path() const { return mgr()->path(); }

void GlobalBookmarkManager::createManager(const QString &filename, const QString &dbusObjectName, CommandHistory* commandHistory) {
    if (m_mgr) {
        kDebug()<<"createManager called twice";
        delete m_mgr;
    }

    kDebug()<<"DBus Object name: "<<dbusObjectName;
    m_mgr = KBookmarkManager::managerForFile(filename, dbusObjectName);

    commandHistory->setBookmarkManager(m_mgr);

    if ( m_model ) {
        m_model->setRoot(root());
    } else {
        m_model = new KBookmarkModel(root(), commandHistory, this);
    }
}

void GlobalBookmarkManager::notifyManagers(const KBookmarkGroup& grp)
{
    m_model->notifyManagers(grp);
}

void GlobalBookmarkManager::notifyManagers() {
    notifyManagers( root() );
}

void GlobalBookmarkManager::reloadConfig() {
    mgr()->emitConfigChanged();
}

QString GlobalBookmarkManager::makeTimeStr(const QString & in)
{
    int secs;
    bool ok;
    secs = in.toInt(&ok);
    if (!ok)
        return QString();
    return makeTimeStr(secs);
}

QString GlobalBookmarkManager::makeTimeStr(int b)
{
    QDateTime dt;
    dt.setTime_t(b);
    return (dt.daysTo(QDateTime::currentDateTime()) > 31)
        ? KGlobal::locale()->formatDate(dt.date())
        : KGlobal::locale()->formatDateTime(dt);
}

#include "moc_globalbookmarkmanager.cpp"
