/*
 *   Copyright (C) 2007 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU Library General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "favicons.h"

#include <KMimeType>
#include <KStandardDirs>
#include <KDebug>

FaviconsEngine::FaviconsEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
}

bool FaviconsEngine::updateSourceEvent(const QString &identifier)
{
    KUrl faviconUrl(identifier);
    if (faviconUrl.protocol().isEmpty()) {
        faviconUrl = KUrl("http://" + identifier);
    }

    const QString fileName = KMimeType::favIconForUrl(faviconUrl.url(), true);
    if (fileName.isEmpty()) {
        kDebug() << "No icon available for " << identifier;
        setData(identifier, QImage());
        return false;
    }

    const QString cachePath = KStandardDirs::locateLocal("cache",  fileName + ".png");
    const QImage image(cachePath, "PNG");
    if (image.isNull()) {
        kWarning() << "Could not load the image" << cachePath;
        setData(identifier, QImage());
        return false;
    }

    setData(identifier, image);
    return true;
}

bool FaviconsEngine::sourceRequestEvent(const QString &identifier)
{
    setData(identifier, QImage());
    return updateSourceEvent(identifier);
}

#include "moc_favicons.cpp"
