/*  This file is part of the KDE libraries
    Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
                  2000 Malte Starostik <malte@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "imagecreator.h"
#include "imagecreatorsettings.h"

#include <QImage>
#include <QCheckBox>
#include <kdemacros.h>
#include <klocale.h>

#ifdef HAVE_KEXIV2
#include <libkexiv2/kexiv2.h>
#endif

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new ImageCreator;
    }
}

bool ImageCreator::create(const QString &path, int, int, QImage &img)
{
    // create image preview
    if (!img.load( path ))
        return false;
    if (img.depth() != 32)
        img = img.convertToFormat(img.hasAlphaChannel() ? QImage::Format_ARGB32 : QImage::Format_RGB32);

#ifdef HAVE_KEXIV2
    ImageCreatorSettings* settings = ImageCreatorSettings::self();
    settings->readConfig();
    if (settings->rotate()) {
        KExiv2Iface::KExiv2 exiv(path);
        exiv.rotateExifQImage(img, exiv.getImageOrientation());
    }
#endif

    return true;
}

ThumbCreator::Flags ImageCreator::flags() const
{
    return None;
}


QWidget *ImageCreator::createConfigurationWidget()
{
    QCheckBox *rotateCheckBox = new QCheckBox(i18nc("@option:check", "Rotate the image automatically"));
    rotateCheckBox->setChecked(ImageCreatorSettings::rotate());
    return rotateCheckBox;
}

void ImageCreator::writeConfiguration(const QWidget *configurationWidget)
{
    const QCheckBox *rotateCheckBox = qobject_cast<const QCheckBox*>(configurationWidget);
    if (rotateCheckBox) {
        ImageCreatorSettings* settings = ImageCreatorSettings::self();
        settings->setRotate(rotateCheckBox->isChecked());
        settings->writeConfig();
    }
}