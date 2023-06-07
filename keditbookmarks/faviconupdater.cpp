// -*- indent-tabs-mode:nil -*-
// vim: set ts=4 sts=4 sw=4 et:
/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Kellett <lypanov@kde.org>

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

#include "faviconupdater.h"

#include "bookmarkiterator.h"
#include "toplevel.h"

#include <kdebug.h>
#include <klocale.h>

#include <kio/job.h>

#include <kmimetype.h>
#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <kmimetypetrader.h>

FavIconUpdater::FavIconUpdater(QObject *parent)
    : QObject(parent),
      m_favIconModule("org.kde.kded", "/modules/favicons", QDBusConnection::sessionBus())
{
    connect(
        &m_favIconModule, SIGNAL(iconChanged(QString,QString)),
        this, SLOT(notifyChange(QString,QString))
    );
}

void FavIconUpdater::downloadIcon(const KBookmark &bk)
{
    m_bk = bk;
    const QString url = bk.url().url();
    const QString favicon = KMimeType::favIconForUrl(url);
    if (!favicon.isEmpty()) {
        kDebug() << "got favicon" << favicon;
        m_bk.setIcon(favicon);
        KEBApp::self()->notifyCommandExecuted();
        // kDebug() << "emit done(true)";
        emit done(true);

    } else {
        kDebug() << "no favicon found";
        m_favIconModule.forceDownloadUrlIcon(url);
    }
}

void FavIconUpdater::notifyChange(const QString &url, const QString &iconName)
{
    kDebug() << url << iconName;
    if (m_bk.url().url() == url) {
        // kded module emits empty iconName on error
        if (iconName.isEmpty()) {
            emit done(false);
        } else {
            m_bk.setIcon(iconName);
            emit done(true);
        }
    }
}

#include "moc_faviconupdater.cpp"
