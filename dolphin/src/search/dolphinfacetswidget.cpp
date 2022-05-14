/***************************************************************************
*    Copyright (C) 2012 by Peter Penz <peter.penz19@gmail.com>            *
*                                                                         *
*    This program is free software; you can redistribute it and/or modify *
*    it under the terms of the GNU General Public License as published by *
*    the Free Software Foundation; either version 2 of the License, or    *
*    (at your option) any later version.                                  *
*                                                                         *
*    This program is distributed in the hope that it will be useful,      *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of       *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
*    GNU General Public License for more details.                         *
*                                                                         *
*    You should have received a copy of the GNU General Public License    *
*    along with this program; if not, write to the                        *
*    Free Software Foundation, Inc.,                                      *
*    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA           *
* **************************************************************************/

#include "dolphinfacetswidget.h"

#include <KLocale>
#include <KMimeType>
#include <QButtonGroup>
#include <QCheckBox>
#include <QtCore/qdatetime.h>
#include <QRadioButton>
#include <QHBoxLayout>

DolphinFacetsWidget::DolphinFacetsWidget(QWidget* parent) :
    QWidget(parent),
    m_anyType(0),
    m_documents(0),
    m_images(0),
    m_audio(0),
    m_videos(0),
    m_type("")
{
    m_anyType   = createRadioButton(i18nc("@option:check", "Any"), this);
    m_documents = createRadioButton(i18nc("@option:check", "Documents"), this);
    m_images    = createRadioButton(i18nc("@option:check", "Images"), this);
    m_audio     = createRadioButton(i18nc("@option:check", "Audio Files"), this);
    m_videos    = createRadioButton(i18nc("@option:check", "Videos"), this);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(m_anyType);
    topLayout->addStretch();
    topLayout->addWidget(m_documents);
    topLayout->addStretch();
    topLayout->addWidget(m_images);
    topLayout->addStretch();
    topLayout->addWidget(m_audio);
    topLayout->addStretch();
    topLayout->addWidget(m_videos);
    topLayout->addStretch();
}

DolphinFacetsWidget::~DolphinFacetsWidget()
{
}

const QString DolphinFacetsWidget::types() {
    return m_type;
}

void DolphinFacetsWidget::facetChange()
{
    // most of the types referenced from http://en.wikipedia.org/wiki/Internet_media_type
    if (m_documents->isChecked()) {
        m_type = "application/x-dvi;application/postscript;application/pdf;image/x-eps";
    } else if (m_images->isChecked()) {
        m_type = "";
        foreach (const KMimeType::Ptr &mime, KMimeType::allMimeTypes()) {
            if (mime->name().startsWith("image/")) {
                m_type.append(mime->name());
                m_type.append(";");
            }
        }
        m_type.chop(1);
    } else if (m_audio->isChecked()) {
        m_type = "";
        foreach (const KMimeType::Ptr &mime, KMimeType::allMimeTypes()) {
            if (mime->name().startsWith("audio/")) {
                m_type.append(mime->name());
                m_type.append(";");
            }
        }
        m_type.chop(1);
    } else if (m_videos->isChecked()) {
        m_type = "";
        foreach (const KMimeType::Ptr &mime, KMimeType::allMimeTypes()) {
            if (mime->name().startsWith("video/")) {
                m_type.append(mime->name());
                m_type.append(";");
            }
        }
        m_type.chop(1);
    } else {
        m_type = "";
    }
}

QRadioButton* DolphinFacetsWidget::createRadioButton(const QString& text,
                                                     QWidget* parent)
{
    QRadioButton* button = new QRadioButton(text, parent);
    connect(button, SIGNAL(clicked()), this, SIGNAL(facetChanged()));
    connect(button, SIGNAL(toggled(bool)), this, SLOT(facetChange()));
    return button;
}

#include "moc_dolphinfacetswidget.cpp"
