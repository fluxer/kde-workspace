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

#include "epubthumbnail.h"

#include <QFile>
#include <QImage>
#include <kdebug.h>
#include <kdemacros.h>

#include <epub.h>

// for reference:
// https://idpf.org/epub/30/spec/epub30-publications.html

static const char* const s_coverpaths[] = {
    "cover.png",
    "cover.jpg",
    "cover.jpeg",
    "cover_image.png",
    "cover_image.jpg",
    "cover_image.jpeg",
    nullptr
};

static const char* const s_contentpaths[] = {
    "content.opf",
    "OEBPS/content.opf",
    nullptr
};

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new EPUBCreator();
    }
}

EPUBCreator::EPUBCreator()
{
}

bool EPUBCreator::create(const QString &path, int width, int height, QImage &img)
{
    Q_UNUSED(width);
    Q_UNUSED(height);

    const QByteArray pathbytes = QFile::encodeName(path);
    struct epub *epubdocument = epub_open(pathbytes.constData(), 1);
    if (!epubdocument) {
        kWarning() << "Could not open" << pathbytes;
        return false;
    }

    int contentpathcounter = 0;
    while (s_contentpaths[contentpathcounter]) {
        char *epubdata = nullptr;
        int epubresult = epub_get_data(epubdocument, s_contentpaths[contentpathcounter], &epubdata);
        if (epubresult > 0) {
            const QString containerstring = QString::fromAscii(epubdata);
            ::free(epubdata);
            QRegExp coverregexp("(id=\"cover\" href=\"([^\\s]+)\" media-type=)");
            if (coverregexp.indexIn(containerstring) != -1) {
                const QByteArray coverhref = coverregexp.capturedTexts()[2].toAscii();
                if (!coverhref.isEmpty()) {
                    kDebug() << "Found cover reference for" << pathbytes << coverhref;
                    epubdata = nullptr;
                    epubresult = epub_get_data(epubdocument, coverhref.constData(), &epubdata);
                    if (epubresult > 0) {
                        img.loadFromData(epubdata, epubresult);
                        ::free(epubdata);
                        epub_close(epubdocument);
                        return !img.isNull();
                    } else {
                        kDebug() << "Could not get cover data for" << pathbytes;
                    }
                }
            } else {
                kDebug() << "Could not find cover reference for" << pathbytes;
            }
        } else {
            kDebug() << "Could not get" << s_contentpaths[contentpathcounter] << "for" << pathbytes;
        }

        contentpathcounter++;
    }

    int coverpathcounter = 0;
    while (s_coverpaths[coverpathcounter]) {
        char *epubdata = nullptr;
        int epubresult = epub_get_data(epubdocument, s_coverpaths[coverpathcounter], &epubdata);
        if (!epubdata || epubresult < 1) {
            kDebug() << "Could not get" << s_coverpaths[coverpathcounter] << "for" << pathbytes;
            coverpathcounter++;
            continue;
        }
        img.loadFromData(epubdata, epubresult);
        ::free(epubdata);
        epub_close(epubdocument);
        return !img.isNull();
    }

    epub_close(epubdocument);
    kDebug() << "No cover found for" << pathbytes;
    return false;
}

ThumbCreator::Flags EPUBCreator::flags() const
{
    return ThumbCreator::None;
}
