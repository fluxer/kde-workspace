/*
    icoutils_common.cpp - Extract Microsoft Window icons and images using icoutils package

    Copyright (c) 2009-2010 by Pali Rohár <pali.rohar@gmail.com>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU General Public                   *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include "icoutils.h"

#include <QList>
#include <QString>
#include <QTemporaryFile>
#include <QImage>
#include <QImageReader>
#include <QPair>
#include <QProcess>

typedef QPair < QString, int > IconInExe;

bool IcoUtils::loadIcoImageFromExe(const QString &inputFileName, QImage &image, int needWidth, int needHeight)
{
    QProcess wrestool;
    wrestool.start("wrestool", QStringList() << "-l" << inputFileName);
    wrestool.waitForFinished();

    if ( wrestool.exitCode() != 0 )
        return false;

    const QStringList output = QString(wrestool.readAll()).split('\n');

    QRegExp regExp("--type=(.*) --name=(.*) --language=(.*) \\[(.*)\\]");

    QList <IconInExe> icons;

    // First try use group icons (type 14, default first for windows executables), then icons (type 3), then group cursors (type 12) and finaly cursors (type 1)
    // Note: Last icon (type 3) could be in higher resolution

    // Group Icons
    foreach ( const QString &line, output )
        if ( regExp.indexIn(line) != -1 && regExp.cap(1).toInt() == 14 )
            icons << qMakePair(regExp.cap(2), 14);

    // Icons
    foreach ( const QString &line, output )
        if ( regExp.indexIn(line) != -1 && regExp.cap(1).toInt() == 3 )
            icons << qMakePair(regExp.cap(2), 3);

    if ( icons.isEmpty() )
        return false;

    foreach ( const IconInExe &icon, icons )
    {

        QString name = icon.first;
        int type = icon.second;

        if ( name.at(0) == '\'' )
            name = name.mid(1, name.size()-2);


        QTemporaryFile outputFile;

        if ( ! outputFile.open() )
            return false;

        QFile(outputFile.fileName()).resize(0);

        wrestool.start("wrestool", QStringList() << "-x" << "-t" << QString::number(type) << "-n" << name << inputFileName << "-o" << outputFile.fileName());
        wrestool.waitForFinished();

        if ( wrestool.exitCode() == 0 && QFile(outputFile.fileName()).size() != 0 )
            return IcoUtils::loadIcoImage(outputFile.fileName(), image, needWidth, needHeight);

    }

    return false;
}

bool IcoUtils::loadIcoImage(const QString &inputFileName, QImage &image, int needWidth, int needHeight)
{
    QImageReader reader(inputFileName, "ico");
    if ( ! reader.canRead() )
        return false;

    QList <QImage> icons;
    do icons << reader.read();
    while ( reader.jumpToNextImage() );

    if ( icons.empty() )
        return false;

    int min_w = 1024;
    int min_h = 1024;
    int index = icons.size() - 1;


    // we loop in reverse order because QtIcoHandler converts all images to 32-bit depth, and resources are ordered from lower depth to higher depth
    for ( int i_index = icons.size() - 1; i_index >= 0 ; --i_index )
    {

        const QImage &icon = icons.at(i_index);
        int i_width = icon.width();
        int i_height = icon.height();
        int i_w = qAbs(i_width - needWidth);
        int i_h = qAbs(i_height - needHeight);

        if ( i_w < min_w || ( i_w == min_w && i_h < min_h ) )
        {

            min_w = i_w;
            min_h = i_h;
            index = i_index;

        }

    }

    image = icons.at(index);
    return true;
}
