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

#include "djvucreator.h"

#include <QImage>
#include <QProcess>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kdemacros.h>

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new DjVuCreator();
    }
}

bool DjVuCreator::create(const QString &path, int width, int height, QImage &img)
{
    const QString ddjvuexe = KStandardDirs::findExe("ddjvu");
    if (ddjvuexe.isEmpty()) {
        kWarning() << "DjVu is not available";
        return false;
    }

    const QStringList ddjvuargs = QStringList()
        << QLatin1String("-page=1")
        << QLatin1String("-format=ppm")
        << (QLatin1String("-size=") + QString::number(width) + QLatin1Char('x') + QString::number(height))
        << path;
    QProcess ddjvuprocess;
    ddjvuprocess.start(ddjvuexe, ddjvuargs);
    if (ddjvuprocess.waitForFinished() == false || ddjvuprocess.exitCode() != 0) {
        kWarning() << ddjvuprocess.readAllStandardError();
        return false;
    }
    const QByteArray ddjvuimage = ddjvuprocess.readAllStandardOutput();

    if (img.loadFromData(ddjvuimage, "PPM") == false) {
        kWarning() << "Could not load ddjvu image";
        return false;
    }

    return true;
}

ThumbCreator::Flags DjVuCreator::flags() const
{
    return ThumbCreator::DrawFrame;
}
