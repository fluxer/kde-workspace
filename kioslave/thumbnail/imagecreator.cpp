/*  This file is part of the KDE project
    Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>

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

#include "imagecreator.h"
#include "imagecreatorsettings.h"

#include <QImage>
#include <QCheckBox>
#include <kdemacros.h>
#include <klocale.h>
#include <kexiv2.h>

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new ImageCreator();
    }
}

ImageCreator::ImageCreator()
{
}

bool ImageCreator::create(const QString &path, int width, int height, QImage &img)
{
    // use preview from Exiv2 metadata if possible
    KExiv2 exiv(path);
    img = exiv.preview();

    // if the thumbnail from Exiv2 metadata is smaller than the request discard it, the preview in
    // file properties dialog for example often requests large thumbnails
    if (!img.isNull() && (img.width() < width || img.height() < height)) {
        img = QImage();
    }

    // create image preview otherwise
    if (img.isNull() && !img.load(path)) {
        return false;
    }

    ImageCreatorSettings* settings = ImageCreatorSettings::self();
    settings->readConfig();
    if (settings->rotate()) {
        exiv.rotateImage(img);
    }

    return true;
}

ThumbCreator::Flags ImageCreator::flags() const
{
    return ThumbCreator::None;
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