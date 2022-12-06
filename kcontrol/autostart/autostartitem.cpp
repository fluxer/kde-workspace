/***************************************************************************
 *   Copyright (C) 2008 by Montel Laurent <montel@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA          *
 ***************************************************************************/

#include "autostartitem.h"
#include "autostart.h"

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QDir>

#include <KDebug>
#include <KIO/CopyJob>

AutoStartItem::AutoStartItem(const QString &service, QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
{
    m_fileName = KUrl(service);
    setCheckState(Autostart::COL_STATUS, Qt::Checked);
}

AutoStartItem::~AutoStartItem()
{
}

KUrl AutoStartItem::fileName() const
{
    return m_fileName;
}

void AutoStartItem::setPath(const QString &path)
{
    Q_ASSERT(path.endsWith(QDir::separator()));

    if (path == m_fileName.directory(KUrl::AppendTrailingSlash)) {
        return;
    }

    const QString& newFileName = path + m_fileName.fileName();
    KIO::move(m_fileName, KUrl(newFileName));

    m_fileName = KUrl(newFileName);
}
