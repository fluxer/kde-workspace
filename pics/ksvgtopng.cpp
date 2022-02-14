/*  This file is part of the KDE Project
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

#include <QApplication>
#include <QImageReader>
#include <QImage>
#include <QDebug>

#include <stdlib.h>

int main(int argc, char **argv)
{
    // Initialize Qt application, otherwise for some svg files it can segfault with:
    // ASSERT failure in QFontDatabase: "A QApplication object needs to be 
    // constructed before FontConfig is used."
    QApplication app(argc, argv);

    if (argc < 5) {
        qDebug() << "Usage : ksvgtopng width height svgfilename outputfilename";
        qDebug() << "Please use full path name for svgfilename";
        return 1;
    }

    const int width = atoi(argv[1]);
    const int height = atoi(argv[2]);
    if (width <= 0) {
        qWarning() << "Width cannot be negative or zero";
        return 2;
    } else if (height <= 0) {
        qWarning() << "Height cannot be negative or zero";
        return 2;
    }

    QImageReader imagereader(argv[3], "SVG");
    imagereader.setScaledSize(QSize(width, height));
    QImage image = imagereader.read();
    if (image.isNull()) {
        qWarning() << "Cannot not read" << argv[3] << ":" << imagereader.errorString();
        return 3;
    }

    if (image.save(argv[4], "PNG") == false) {
        qWarning() << "Could not save" << argv[4];
        return 4;
    }

    return 0;
}
