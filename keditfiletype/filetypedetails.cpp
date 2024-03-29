/* This file is part of the KDE project
   Copyright (C) 2000, 2007 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published by
   the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Own
#include "filetypedetails.h"

// Qt
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLayout>
#include <QListWidget>
#include <QRadioButton>
#include <QLabel>

// KDE
#include <kconfig.h>
#include <kdebug.h>
#include <kicondialog.h>
#include <kinputdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kicon.h>
#include <kpushbutton.h>

// Local
#include "kservicelistwidget.h"
#include "typeslistitem.h"

FileTypeDetails::FileTypeDetails( QWidget * parent )
    : QWidget( parent ), m_mimeTypeData(0), m_item(0)
{

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    m_mimeTypeLabel = new QLabel(this);
    topLayout->addWidget(m_mimeTypeLabel, 0, Qt::AlignCenter);

    m_tabWidget = new QTabWidget(this);
    topLayout->addWidget(m_tabWidget);

  QString wtstr;
  // First tab - General
  QWidget * firstWidget = new QWidget(m_tabWidget);
  QVBoxLayout *firstLayout = new QVBoxLayout(firstWidget);

  QHBoxLayout *hBox = new QHBoxLayout();
  firstLayout->addLayout(hBox);

  iconButton = new KIconButton(firstWidget);
  iconButton->setIconType(KIconLoader::Desktop, KIconLoader::MimeType);
  connect(iconButton, SIGNAL(iconChanged(QString)), SLOT(updateIcon(QString)));
  iconButton->setWhatsThis( i18n("This button displays the icon associated"
                                " with the selected file type. Click on it to choose a different icon.") );
  iconButton->setFixedSize(70, 70);
  hBox->addWidget(iconButton);

  QGroupBox *gb = new QGroupBox(i18n("Filename Patterns"), firstWidget);
  hBox->addWidget(gb);

  hBox = new QHBoxLayout(gb);

  extensionLB = new QListWidget(gb);
  connect(extensionLB, SIGNAL(itemSelectionChanged()), SLOT(enableExtButtons()));
  hBox->addWidget(extensionLB);

  extensionLB->setFixedHeight(extensionLB->minimumSizeHint().height());


  extensionLB->setWhatsThis( i18n("This box contains a list of patterns that can be"
    " used to identify files of the selected type. For example, the pattern *.txt is"
    " associated with the file type 'text/plain'; all files ending in '.txt' are recognized"
    " as plain text files.") );

  QVBoxLayout *vbox = new QVBoxLayout();
  hBox->addLayout(vbox);

  addExtButton = new KPushButton(i18n("Add..."), gb);
  addExtButton->setIcon(KIcon("list-add"));
  addExtButton->setEnabled(false);
  connect(addExtButton, SIGNAL(clicked()),
          this, SLOT(addExtension()));
  vbox->addWidget(addExtButton);
  addExtButton->setWhatsThis( i18n("Add a new pattern for the selected file type.") );

  removeExtButton = new KPushButton(i18n("Remove"), gb);
  removeExtButton->setIcon(KIcon("list-remove"));
  removeExtButton->setEnabled(false);
  connect(removeExtButton, SIGNAL(clicked()),
          this, SLOT(removeExtension()));
  vbox->addWidget(removeExtButton);
  removeExtButton->setWhatsThis( i18n("Remove the selected filename pattern.") );

  vbox->addStretch(1);

  gb->setFixedHeight(gb->minimumSizeHint().height());

  description = new KLineEdit(firstWidget);
  description->setClearButtonShown(true);
  connect(description, SIGNAL(textChanged(const QString &)),
          SLOT(updateDescription(const QString &)));

  QHBoxLayout *descriptionBox = new QHBoxLayout;
  descriptionBox->addWidget(new QLabel(i18n("Description:"),firstWidget));
  descriptionBox->addWidget(description);
  firstLayout->addLayout(descriptionBox);

  wtstr = i18n("You can enter a short description for files of the selected"
    " file type (e.g. 'HTML Page'). This description will be used by applications"
    " like Konqueror to display directory content.");
  description->setWhatsThis( wtstr );

  serviceListWidget = new KServiceListWidget( KServiceListWidget::SERVICELIST_APPLICATIONS, firstWidget );
  connect( serviceListWidget, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
  firstLayout->addWidget(serviceListWidget,5);

  // Second tab - Embedding
  QWidget * secondWidget = new QWidget(m_tabWidget);
  QVBoxLayout *secondLayout = new QVBoxLayout(secondWidget);

  m_autoEmbedBox = new QGroupBox( i18n("Left Click Action in Konqueror"), secondWidget );
  secondLayout->addWidget( m_autoEmbedBox );

  m_autoEmbedBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

  QRadioButton *embViewerRadio = new QRadioButton( i18n("Show file in embedded viewer") );
  QRadioButton *sepViewerRadio = new QRadioButton( i18n("Show file in separate viewer") );
  m_rbGroupSettings = new QRadioButton( QString("Use settings for '%1' group") );

  m_chkAskSave = new QCheckBox( i18n("Ask whether to save to disk instead (only for Konqueror browser)") );
  connect(m_chkAskSave, SIGNAL( toggled(bool) ), SLOT( slotAskSaveToggled(bool) ));

  m_autoEmbedGroup = new QButtonGroup(m_autoEmbedBox);
  m_autoEmbedGroup->addButton(embViewerRadio, 0);
  m_autoEmbedGroup->addButton(sepViewerRadio, 1);
  m_autoEmbedGroup->addButton(m_rbGroupSettings, 2);
  connect(m_autoEmbedGroup, SIGNAL( buttonClicked(int) ), SLOT( slotAutoEmbedClicked(int) ));

  vbox = new QVBoxLayout(m_autoEmbedBox);
  vbox->addWidget(embViewerRadio);
  vbox->addWidget(sepViewerRadio);
  vbox->addWidget(m_rbGroupSettings);
  vbox->addWidget(m_chkAskSave);

  m_autoEmbedBox->setWhatsThis( i18n("Here you can configure what the Konqueror file manager"
    " will do when you click on a file of this type. Konqueror can either display the file in"
    " an embedded viewer, or start up a separate application. If set to 'Use settings for G group',"
    " the file manager will behave according to the settings of the group G to which this type belongs;"
    " for instance, 'image' if the current file type is image/png. Dolphin"
    " always shows files in a separate viewer.") );

  embedServiceListWidget = new KServiceListWidget( KServiceListWidget::SERVICELIST_SERVICES, secondWidget );
//  embedServiceListWidget->setMinimumHeight( serviceListWidget->sizeHint().height() );
  connect( embedServiceListWidget, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
  secondLayout->addWidget(embedServiceListWidget);

  m_tabWidget->addTab( firstWidget, i18n("&General") );
  m_tabWidget->addTab( secondWidget, i18n("&Embedding") );
}

void FileTypeDetails::updateRemoveButton()
{
    removeExtButton->setEnabled(extensionLB->count()>0);
}

void FileTypeDetails::updateIcon(const QString &icon)
{
  if (!m_mimeTypeData)
    return;

  m_mimeTypeData->setUserSpecifiedIcon(icon);

  if (m_item)
      m_item->setIcon(icon);

  emit changed(true);
}

void FileTypeDetails::updateDescription(const QString &desc)
{
  if (!m_mimeTypeData)
    return;

  m_mimeTypeData->setComment(desc);

  emit changed(true);
}

void FileTypeDetails::addExtension()
{
  if ( !m_mimeTypeData )
    return;

  bool ok;
  QString ext = KInputDialog::getText( i18n( "Add New Extension" ),
    i18n( "Extension:" ), "*.", &ok, this );
  if (ok) {
    extensionLB->addItem(ext);
    QStringList patt = m_mimeTypeData->patterns();
    patt += ext;
    m_mimeTypeData->setPatterns(patt);
    updateRemoveButton();
    emit changed(true);
  }
}

void FileTypeDetails::removeExtension()
{
  if (extensionLB->currentRow() == -1)
    return;
  if ( !m_mimeTypeData )
    return;
  QStringList patt = m_mimeTypeData->patterns();
  patt.removeAll(extensionLB->currentItem()->text());
  m_mimeTypeData->setPatterns(patt);
  delete extensionLB->takeItem(extensionLB->currentRow());
  updateRemoveButton();
  emit changed(true);
}

void FileTypeDetails::slotAutoEmbedClicked( int button )
{
  if ( !m_mimeTypeData || (button > 2))
    return;

  m_mimeTypeData->setAutoEmbed( (MimeTypeData::AutoEmbed) button );

  updateAskSave();

  emit changed(true);
}

void FileTypeDetails::updateAskSave()
{
    if ( !m_mimeTypeData )
        return;

    MimeTypeData::AutoEmbed autoEmbed = m_mimeTypeData->autoEmbed();
    if (m_mimeTypeData->isMeta() && autoEmbed == MimeTypeData::UseGroupSetting) {
        // Resolve by looking at group (we could cache groups somewhere to avoid the re-parsing?)
        autoEmbed = MimeTypeData(m_mimeTypeData->majorType()).autoEmbed();
    }

    const QString mimeType = m_mimeTypeData->name();

    QString dontAskAgainName;
    if (autoEmbed == MimeTypeData::Yes) // Embedded
        dontAskAgainName = "askEmbedOrSave"+mimeType;
    else
        dontAskAgainName = "askSave"+mimeType;

    KSharedConfig::Ptr config = KSharedConfig::openConfig("filetypesrc", KConfig::NoGlobals);
    // default value
    bool ask = config->group("Notification Messages").readEntry(dontAskAgainName, QString()).isEmpty();
    // per-mimetype override if there's one
    m_mimeTypeData->getAskSave(ask);

    bool neverAsk = false;

    if (autoEmbed == MimeTypeData::Yes) {
        const KMimeType::Ptr mime = KMimeType::mimeType( mimeType );
        if (mime) {
            // SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC SYNC
            // NOTE: Keep this function in sync with
            // kdelibs/kparts/browseropenorsavequestion.cpp BrowserOpenOrSaveQuestionPrivate::autoEmbedMimeType

            // Don't ask for:
            // - html (even new tabs would ask, due to about:blank!)
            // - dirs obviously (though not common over HTTP :),
            // - images (reasoning: no need to save, most of the time, because fast to see)
            // e.g. postscript is different, because takes longer to read, so
            // it's more likely that the user might want to save it.
            // - multipart/* ("server push", see kmultipart)
            if ( mime->is( "text/html" ) ||
                 mime->is( "application/xml" ) ||
                 mime->is( "inode/directory" ) ||
                 mimeType.startsWith( QLatin1String("image") ) ||
                 mime->is( "multipart/x-mixed-replace" ) ||
                 mime->is( "multipart/replace" ) )
            {
                neverAsk = true;
            }
        }
    }

    m_chkAskSave->blockSignals(true);
    m_chkAskSave->setChecked(ask && !neverAsk);
    m_chkAskSave->setEnabled(!neverAsk);
    m_chkAskSave->blockSignals(false);
}

void FileTypeDetails::slotAskSaveToggled(bool askSave)
{
    if (!m_mimeTypeData)
        return;

    m_mimeTypeData->setAskSave(askSave);
    emit changed(true);
}

void FileTypeDetails::setMimeTypeData( MimeTypeData * mimeTypeData, TypesListItem* item )
{
  m_mimeTypeData = mimeTypeData;
  m_item = item; // can be 0
  Q_ASSERT(mimeTypeData);
  m_mimeTypeLabel->setText(i18n("File type %1", mimeTypeData->name()));
  iconButton->setIcon(mimeTypeData->icon());
  description->setText(mimeTypeData->comment());
  m_rbGroupSettings->setText( i18n("Use settings for '%1' group", mimeTypeData->majorType() ) );
  extensionLB->clear();
  addExtButton->setEnabled(true);
  removeExtButton->setEnabled(false);

  serviceListWidget->setMimeTypeData(mimeTypeData);
  embedServiceListWidget->setMimeTypeData(mimeTypeData);
  m_autoEmbedGroup->button(mimeTypeData->autoEmbed())->setChecked(true);
  m_rbGroupSettings->setEnabled(true);

  extensionLB->addItems(mimeTypeData->patterns());

  updateAskSave();
}

void FileTypeDetails::enableExtButtons()
{
  removeExtButton->setEnabled(true);
}

void FileTypeDetails::refresh()
{
    if (!m_mimeTypeData)
        return;

    // Called when ksycoca has been updated -> refresh data, then widgets
    m_mimeTypeData->refresh();
    setMimeTypeData(m_mimeTypeData, m_item);
}

#include "moc_filetypedetails.cpp"
