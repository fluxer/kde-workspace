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
    connect(&m_favIconModule, SIGNAL(iconChanged(bool,QString,QString)),
            this, SLOT(notifyChange(bool,QString,QString)) );
    connect(&m_favIconModule, SIGNAL(error(bool,QString,QString)),
            this, SLOT(slotFavIconError(bool,QString,QString)) );
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
        emit done(true, QString());

    } else {
        kDebug() << "no favicon found";
        m_favIconModule.forceDownloadHostIcon(url);
    }
}

FavIconUpdater::~FavIconUpdater()
{
}

// webkit callback
void FavIconUpdater::setIconUrl(const KUrl &iconURL)
{
    m_favIconModule.setIconForUrl(m_bk.url().url(), iconURL.url());
    // The above call will make the kded module start the download and emit iconChanged or error.
}

bool FavIconUpdater::isFavIconSignalRelevant(bool isHost, const QString& hostOrURL) const
{
    // Is this signal interesting to us? (Don't react on an unrelated favicon)
    return (isHost && hostOrURL == m_bk.url().host()) ||
        (!isHost && hostOrURL == m_bk.url().url()); // should we use the api that ignores trailing slashes?
}

void FavIconUpdater::notifyChange(bool isHost,
                                  const QString& hostOrURL,
                                  const QString& iconName)
{
    kDebug() << hostOrURL << iconName;
    if (isFavIconSignalRelevant(isHost, hostOrURL)) {
        if (iconName.isEmpty()) { // old version of the kded module could emit with an empty iconName on error
            slotFavIconError(isHost, hostOrURL, QString());
        } else {
            m_bk.setIcon(iconName);
            emit done(true, QString());
        }
    }
}

void FavIconUpdater::slotFavIconError(bool isHost, const QString& hostOrURL, const QString& errorString)
{
    kDebug() << hostOrURL << errorString;
    if (isFavIconSignalRelevant(isHost, hostOrURL)) {
        emit done(false, errorString);
    }
}

#include "moc_faviconupdater.cpp"
