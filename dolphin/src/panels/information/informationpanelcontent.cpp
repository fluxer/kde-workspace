/***************************************************************************
 *   Copyright (C) 2009 by Peter Penz <peter.penz19@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "informationpanelcontent.h"

#include <KDialog>
#include <KFileItem>
#include <KGlobalSettings>
#include <KIconLoader>
#include <KLocale>
#include <KMenu>
#include <KSeparator>
#include <KStringHandler>
#include <KImageFilePreview>
#include <KMediaWidget>
#include <KFileMetaDataWidget>

#include <panels/places/placesitem.h>
#include <panels/places/placesitemmodel.h>

#include <QEvent>
#include <QLabel>
#include <QPixmap>
#include <QPointer>
#include <QtGui/qevent.h>
#include <QScrollArea>
#include <QTextDocument>
#include <QTextLayout>
#include <QtGui/qtextlayout.h>
#include <QTimer>
#include <QVBoxLayout>

#include "dolphin_informationpanelsettings.h"
#include "filemetadataconfigurationdialog.h"

InformationPanelContent::InformationPanelContent(QWidget* parent) :
    QWidget(parent),
    m_item(),
    m_preview(0),
    m_playerWidget(0),
    m_nameLabel(0),
    m_metaDataWidget(0),
    m_metaDataArea(0),
    m_placesItemModel(0)
{
    parent->installEventFilter(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(KDialog::spacingHint());

    // preview
    const int minPreviewWidth = KIconLoader::SizeEnormous + KIconLoader::SizeMedium;

    m_preview = new KImageFilePreview(this);
    m_preview->setMinimumWidth(minPreviewWidth);
    m_preview->setMinimumHeight(KIconLoader::SizeEnormous);

    m_playerWidget = new KMediaWidget(this);
    m_playerWidget->player()->setPlayerID("informationpanel");
    m_playerWidget->hide();
    m_playerWidget->setMinimumWidth(minPreviewWidth);
    m_playerWidget->setMinimumHeight(KIconLoader::SizeEnormous);

    // name
    m_nameLabel = new QLabel(this);
    QFont font = m_nameLabel->font();
    font.setBold(true);
    m_nameLabel->setFont(font);
    m_nameLabel->setTextFormat(Qt::PlainText);
    m_nameLabel->setAlignment(Qt::AlignHCenter);
    m_nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_preview->setVisible(InformationPanelSettings::previewsShown());

    m_metaDataWidget = new KFileMetaDataWidget(this);
    m_metaDataWidget->setFont(KGlobalSettings::smallestReadableFont());
    m_metaDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    connect(m_metaDataWidget, SIGNAL(urlActivated(KUrl)), this, SIGNAL(urlActivated(KUrl)));

    // Encapsulate the MetaDataWidget inside a container that has a dummy widget
    // at the bottom. This prevents that the meta data widget gets vertically stretched
    // in the case where the height of m_metaDataArea > m_metaDataWidget.
    QWidget* metaDataWidgetContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(metaDataWidgetContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    containerLayout->addWidget(m_metaDataWidget);
    containerLayout->addStretch();

    m_metaDataArea = new QScrollArea(this);
    m_metaDataArea->setWidget(metaDataWidgetContainer);
    m_metaDataArea->setWidgetResizable(true);
    m_metaDataArea->setFrameShape(QFrame::NoFrame);

    QWidget* viewport = m_metaDataArea->viewport();
    viewport->installEventFilter(this);

    QPalette palette = viewport->palette();
    palette.setColor(viewport->backgroundRole(), QColor(Qt::transparent));
    viewport->setPalette(palette);

    layout->addWidget(m_preview);
    layout->addWidget(m_playerWidget);
    layout->addWidget(m_nameLabel);
    layout->addWidget(new KSeparator());
    layout->addWidget(m_metaDataArea);

    m_placesItemModel = new PlacesItemModel(this);
}

InformationPanelContent::~InformationPanelContent()
{
    InformationPanelSettings::self()->writeConfig();
}

void InformationPanelContent::showItem(const KFileItem& item)
{
    // qDebug() << Q_FUNC_INFO << item.url();

    const KUrl itemUrl = item.url();
    const bool isSearchUrl = itemUrl.protocol().contains("search") && item.localPath().isEmpty();
    if (!applyPlace(itemUrl)) {
        setNameLabelText(item.text());
        m_preview->clearPreview();
        if (isSearchUrl) {
            // in the case of a search-URL the URL is not readable for humans
            // (at least not useful to show in the Information Panel)
            QString iconPath = KIconLoader::global()->iconPath("nepomuk", -KIconLoader::SizeEnormous, false);
            // qDebug() << Q_FUNC_INFO << iconPath;
            m_preview->showPreview(KUrl(iconPath));
        } else {
            m_preview->showPreview(itemUrl);
        }
    }

    if (m_metaDataWidget) {
        m_metaDataWidget->show();
        m_metaDataWidget->setItems(KFileItemList() << item);
    }

    if (InformationPanelSettings::previewsShown()) {
        const QString mimeType = item.mimetype();
        if (m_playerWidget->player()->isMimeSupported(mimeType)) {
            m_preview->hide();
            m_playerWidget->show();
            m_playerWidget->open(item.targetUrl().url());
            m_playerWidget->setPlay(int(1));
        } else {
            m_playerWidget->setPlay(int(0));
            m_playerWidget->hide();
            m_preview->show();
        }
    } else {
        m_playerWidget->setPlay(int(0));
        m_playerWidget->hide();
    }

    m_item = item;
}

void InformationPanelContent::showItems(const KFileItemList& items)
{
    // qDebug() << Q_FUNC_INFO << items;

    m_preview->clearPreview();
    QString iconPath = KIconLoader::global()->iconPath("dialog-information", -KIconLoader::SizeEnormous, false);
    // qDebug() << Q_FUNC_INFO << iconPath;
    m_preview->showPreview(KUrl(iconPath));
    setNameLabelText(i18ncp("@label", "%1 item selected", "%1 items selected", items.count()));

    if (m_metaDataWidget) {
        m_metaDataWidget->setItems(items);
    }

    m_playerWidget->setPlay(int(0));
    m_playerWidget->hide();

    m_item = KFileItem();
}

bool InformationPanelContent::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Resize: {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        if (obj == m_metaDataArea->viewport()) {
            // The size of the meta text area has changed. Adjust the fixed
            // width in a way that no horizontal scrollbar needs to be shown.
            m_metaDataWidget->setFixedWidth(resizeEvent->size().width());
        } else if (obj == parent()) {
            adjustWidgetSizes(resizeEvent->size().width());
        }
        break;
    }

    case QEvent::Polish:
        adjustWidgetSizes(parentWidget()->width());
        break;

    case QEvent::FontChange:
        m_metaDataWidget->setFont(KGlobalSettings::smallestReadableFont());
        break;

    case QEvent::Hide:
        if (m_playerWidget)
            m_playerWidget->setPlay(int(0));
        break;

    default:
        break;
    }

    return QWidget::eventFilter(obj, event);
}

void InformationPanelContent::configureSettings(const QList<QAction*>& customContextMenuActions)
{
    KMenu popup(this);

    QAction* previewAction = popup.addAction(i18nc("@action:inmenu", "Preview"));
    previewAction->setIcon(KIcon("view-preview"));
    previewAction->setCheckable(true);
    previewAction->setChecked(InformationPanelSettings::previewsShown());

    QAction* configureAction = popup.addAction(i18nc("@action:inmenu", "Configure..."));
    configureAction->setIcon(KIcon("configure"));

    popup.addSeparator();
    foreach (QAction* action, customContextMenuActions) {
        popup.addAction(action);
    }

    // Open the popup and adjust the settings for the
    // selected action.
    QAction* action = popup.exec(QCursor::pos());
    if (!action) {
        return;
    }

    const bool isChecked = action->isChecked();
    if (action == previewAction) {
        if (!isChecked) {
            m_playerWidget->setPlay(int(0));
            m_playerWidget->hide();
        }
        m_preview->setVisible(isChecked);
        InformationPanelSettings::setPreviewsShown(isChecked);
    } else if (action == configureAction) {
        FileMetaDataConfigurationDialog* dialog = new FileMetaDataConfigurationDialog();
        dialog->setDescription(i18nc("@label::textbox",
                                     "Select which data should be shown in the information panel:"));
        dialog->setItems(m_metaDataWidget->items());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
        connect(dialog, SIGNAL(destroyed()), this, SLOT(refreshMetaData()));
    }
}

void InformationPanelContent::refreshMetaData()
{
    if (!m_item.isNull()) {
        showItem(m_item);
    }
}

bool InformationPanelContent::applyPlace(const KUrl& url)
{
    const int count = m_placesItemModel->count();
    for (int i = 0; i < count; ++i) {
        const PlacesItem* item = m_placesItemModel->placesItem(i);
        if (item->url().equals(url, KUrl::RemoveTrailingSlash)) {
            setNameLabelText(item->text());
            m_preview->clearPreview();
            QString iconPath = KIconLoader::global()->iconPath(item->icon(), -KIconLoader::SizeEnormous, false);
            // qDebug() << Q_FUNC_INFO << iconPath << item->icon();
            m_preview->showPreview(KUrl(iconPath));
            return true;
        }
    }

    return false;
}

void InformationPanelContent::setNameLabelText(const QString& text)
{
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    const QString processedText = Qt::mightBeRichText(text) ? text : KStringHandler::preProcessWrap(text);

    QTextLayout textLayout(processedText);
    textLayout.setFont(m_nameLabel->font());
    textLayout.setTextOption(textOption);

    QString wrappedText;
    wrappedText.reserve(processedText.length());

    // wrap the text to fit into the width of m_nameLabel
    textLayout.beginLayout();
    QTextLine line = textLayout.createLine();
    while (line.isValid()) {
        line.setLineWidth(m_nameLabel->width());
        wrappedText += processedText.mid(line.textStart(), line.textLength());

        line = textLayout.createLine();
        if (line.isValid()) {
            wrappedText += QChar::LineSeparator;
        }
    }
    textLayout.endLayout();

    m_nameLabel->setText(wrappedText);
}

void InformationPanelContent::adjustWidgetSizes(int width)
{
    // If the text inside the name label or the info label cannot
    // get wrapped, then the maximum width of the label is increased
    // so that the width of the information panel gets increased.
    // To prevent this, the maximum width is adjusted to
    // the current width of the panel.
    const int maxWidth = width - KDialog::spacingHint() * 4;
    m_nameLabel->setMaximumWidth(maxWidth);

    // The metadata widget also contains a text widget which may return
    // a large preferred width.
    if (m_metaDataWidget) {
        m_metaDataWidget->setMaximumWidth(maxWidth);
    }

    // try to increase the preview as large as possible
    m_preview->setMaximumSize(QSize(maxWidth, maxWidth));

    // the size of the video player should match that of the preview size
    m_playerWidget->setMaximumSize(maxWidth, maxWidth);
}

#include "moc_informationpanelcontent.cpp"
