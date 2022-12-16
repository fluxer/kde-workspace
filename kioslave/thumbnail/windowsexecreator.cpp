/*
    windowsexecreator.cpp - Thumbnail Creator for Microsoft Windows Executables

    Copyright (c) 2009 by Pali Rohár <pali.rohar@gmail.com>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU General Public                   *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#include "windowsexecreator.h"

#include <QString>
#include <QImage>
#include <QPair>
#include <QProcess>

#include <ktemporaryfile.h>
#include <kdebug.h>
#include <kdemacros.h>

typedef QPair<QString,int> IconInExe;

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new WindowsExeCreator();
    }
}

WindowsExeCreator::WindowsExeCreator()
{
}

bool WindowsExeCreator::create(const QString &path, int width, int height, QImage &img)
{
    QProcess wrestool;
    wrestool.start("wrestool", QStringList() << "-l" << path);
    wrestool.waitForFinished();

    if (wrestool.exitCode() != 0) {
        return false;
    }

    const QStringList output = QString(wrestool.readAll()).split('\n');

    QRegExp regExp("--type=(.*) --name=(.*) --language=(.*) \\[(.*)\\]");

    QList<IconInExe> icons;

    // First try use group icons (type 14, default first for windows executables)
    foreach (const QString &line, output) {
        if (regExp.indexIn(line) != -1 && regExp.cap(1).toInt() == 14) {
            icons << qMakePair(regExp.cap(2), 14);
        }
    }

    // Then icons (type 3, could be in higher resolution)
    foreach (const QString &line, output) {
        if (regExp.indexIn(line) != -1 && regExp.cap(1).toInt() == 3) {
            icons << qMakePair(regExp.cap(2), 3);
        }
    }

    foreach (const IconInExe &icon, icons) {
        QString name = icon.first;
        const int type = icon.second;

        if (name.at(0) == '\'') {
            name = name.mid(1, name.size() - 2);
        }

        QString outputFile = KTemporaryFile::filePath("XXXXXXXXXX.ico");

        const QStringList wrestoolargs = QStringList()
            << "-x"
            << "-t" << QString::number(type)
            << "-n" << name
            << "-o" << outputFile
            << path;
        wrestool.start("wrestool", wrestoolargs);
        wrestool.waitForFinished();

        if (wrestool.exitCode() == 0 && QFile::exists(outputFile)) {
            const QImage tmp(outputFile, "ICO");
            img = tmp.scaled(QSize(width, height), Qt::KeepAspectRatio);
            QFile::remove(outputFile);
            return true;
        }
    }

    return false;
}
