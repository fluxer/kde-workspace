/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

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

#include "comiccreator.h"

#include <QFile>
#include <kdemacros.h>
#include <karchive.h>
#include <kdebug.h>

#include <sys/stat.h>

// For KIO-Thumbnail debug outputs
#define KIO_THUMB 11371

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new ComicCreator();
    }
}

ComicCreator::ComicCreator()
{
}

bool ComicCreator::create(const QString &path, int width, int height, QImage &img)
{
    Q_UNUSED(width);
    Q_UNUSED(height);

    // Can our archive be opened?
    KArchive karchive(path);
    if (!karchive.isReadable()) {
        kDebug(KIO_THUMB) << "KArchive cannot open the comic book" << karchive.errorString();
        return false;
    }

    // Get and filter the entries from the archive.
    QStringList imageslist;
    foreach (const KArchiveEntry &karchiveentry, karchive.list()) {
        if (!S_ISREG(karchiveentry.mode)) {
            continue;
        }
        const QByteArray lowerpathname = karchiveentry.pathname.toLower();
        if (!lowerpathname.endsWith(".gif") && !lowerpathname.endsWith(".jpg")
            && !lowerpathname.endsWith(".jpeg") && !lowerpathname.endsWith(".png")) {
            continue;
        }
        imageslist.append(QFile::decodeName(karchiveentry.pathname));
    }

    if (imageslist.isEmpty()) {
        kDebug(KIO_THUMB) << "No image found in the comic book";
        return false;
    }

    // Extract the cover file.
    img = QImage::fromData(karchive.data(imageslist.at(0)));
    if (img.isNull()) {
        kWarning(KIO_THUMB) << "Could not get the comic book image" << karchive.errorString();
        return false;
    }

    return true;
}

ThumbCreator::Flags ComicCreator::flags() const
{
    return ThumbCreator::DrawFrame;
}
