/* This file is part of the KDE libraries
   Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
   Copyright (C) 1999 David Faure (faure@kde.org)

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

#include "kdebugconfig.h"

#include <QLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QtDBus/QtDBus>
#include <QFile>
#include <QTextStream>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kseparator.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY(KCMDebugFactory, registerPlugin<KCMDebug>();)
K_EXPORT_PLUGIN(KCMDebugFactory("kcmdebugconfig","kcm_debugconfig"))

KCMDebug::KCMDebug( QWidget* parent, const QVariantList& )
    : KCModule( KCMDebugFactory::componentData(), parent )
{
    setQuickHelp( i18n("<h1>Debug</h1>"
            "This module allows you to change KDE debug preferences."));

    setupUi(this);

    KAboutData *about =
        new KAboutData( I18N_NOOP("KCMDebug"), 0,
                        ki18n("KDE Debug Module"),
                        0, KLocalizedString(), KAboutData::License_GPL,
                        ki18n("Copyright 1999-2009, David Faure <email>faure@kde.org</email>\n"
                            "Copyright 2014-2015, Ivailo Monev <email>xakepa10@gmail.com</email>"
                        ));

    about->addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData( about );

    this->layout()->setContentsMargins(0, 0, 0, 0);

    // Debug area tree
    readAreas();
    m_incrSearch->searchLine()->addTreeWidget(m_areaWidget);

    pConfig = new KConfig( "kdebugrc", KConfig::NoGlobals );

    for( QMap<QString,QString>::const_iterator it = mAreaMap.begin(); it != mAreaMap.end(); ++it ) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_areaWidget, QStringList() << it.value());
        item->setData(0, Qt::UserRole, it.key().simplified());
    }

    QStringList destList;
    destList.append( i18n("File") );
    destList.append( i18n("Message Box") );
    destList.append( i18n("Shell") );
    destList.append( i18n("Syslog") );
    destList.append( i18n("None") );

    connect( m_areaWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            SLOT(slotDebugAreaChanged(QTreeWidgetItem*)) );
    connect(pInfoCombo, SIGNAL(activated(int)),
            this, SLOT(slotDestinationChanged()));
    pInfoCombo->addItems( destList );
    connect(pWarnCombo, SIGNAL(activated(int)),
            this, SLOT(slotDestinationChanged()));
    pWarnCombo->addItems( destList );
    connect(pErrorCombo, SIGNAL(activated(int)),
            this, SLOT(slotDestinationChanged()));
    pErrorCombo->addItems( destList );
    connect(pFatalCombo, SIGNAL(activated(int)),
            this, SLOT(slotDestinationChanged()));
    pFatalCombo->addItems( destList );
    connect(pAbortFatal, SIGNAL(stateChanged(int)),
            this, SLOT(slotAbortFatalChanged()));

    // Hack!
    m_disableAll = m_disableAll2;
    connect(m_disableAll, SIGNAL(toggled(bool)), this, SLOT(slotDisableAllChanged()));

    // Get initial values
    showArea(QString("0"));
}


KCMDebug::~KCMDebug()
{
}

void KCMDebug::readAreas()
{
    // Group 0 is not used anymore. kDebug() uses the area named after the appname.
    //areas.insert( "      0" /*cf rightJustified below*/, "0 (generic)" );

    const QString confAreasFile = KStandardDirs::locate("config", "kdebug.areas");
    QFile file( confAreasFile );
    if (!file.open(QIODevice::ReadOnly)) {
        kWarning() << "Couldn't open" << confAreasFile;
    } else {
        QString data;

        QTextStream ts(&file);
        ts.setCodec( "ISO-8859-1" );
        while (!ts.atEnd()) {
            data = ts.readLine().simplified();

            int pos = data.indexOf("#");
            if ( pos != -1 ) {
                data.truncate( pos );
                data = data.simplified();
            }

            if (data.isEmpty())
                continue;

            const int space = data.indexOf(' ');
            if (space == -1)
                kError() << "No space:" << data << endl;

            bool longOK;
            unsigned long number = data.left(space).toULong(&longOK);
            if (!longOK)
                kError() << "The first part wasn't a number : " << data << endl;

            const QString description = data.mid(space).simplified();

            // In the key, right-align the area number to 6 digits for proper sorting
            const QString key = QString::number(number).rightJustified(6);
            mAreaMap.insert( key, QString("%1 %2").arg(number).arg(description) );
        }
    }

    KConfig config("kdebugrc", KConfig::NoGlobals);
    Q_FOREACH(const QString& groupName, config.groupList()) {
        mAreaMap.insert(groupName, groupName); // ID == description
    }

    return;
}

void KCMDebug::load()
{
    KConfigGroup topGroup(pConfig, QString());
    m_disableAll->setChecked(topGroup.readEntry("DisableAll", true));
    emit changed( false );
}

void KCMDebug::save()
{
    KConfigGroup group = pConfig->group( mCurrentDebugArea ); // Group name = debug area code
    group.writeEntry( "InfoOutput", pInfoCombo->currentIndex() );
    group.writePathEntry( "InfoFilename", pInfoFile->text() );
    //group.writeEntry( "InfoShow", pInfoShow->text() );
    group.writeEntry( "WarnOutput", pWarnCombo->currentIndex() );
    group.writePathEntry( "WarnFilename", pWarnFile->text() );
    //group.writeEntry( "WarnShow", pWarnShow->text() );
    group.writeEntry( "ErrorOutput", pErrorCombo->currentIndex() );
    group.writePathEntry( "ErrorFilename", pErrorFile->text() );
    //group.writeEntry( "ErrorShow", pErrorShow->text() );
    group.writeEntry( "FatalOutput", pFatalCombo->currentIndex() );
    group.writePathEntry( "FatalFilename", pFatalFile->text() );
    //group.writeEntry( "FatalShow", pFatalShow->text() );
    group.writeEntry( "AbortFatal", pAbortFatal->isChecked() );
    KConfigGroup topGroup(pConfig, QString());
    topGroup.writeEntry("DisableAll", m_disableAll->isChecked());
    pConfig->sync();

    QDBusMessage msg = QDBusMessage::createSignal("/", "org.kde.KDebug", "configChanged");
    if (!QDBusConnection::sessionBus().send(msg))
    {
        kError() << "Unable to send D-BUS message" << endl;
    }
    emit changed( false );
}

void KCMDebug::defaults()
{
    // TODO: reset back to default
}

void KCMDebug::slotApply()
{
    save();
    pConfig->sync();
}

void KCMDebug::showArea(const QString& areaName)
{
    /* Fill dialog fields with values from config data */
    mCurrentDebugArea = areaName;
    KConfigGroup group = pConfig->group(areaName);
    pInfoCombo->setCurrentIndex( group.readEntry( "InfoOutput", 2 ) );
    pInfoFile->setText( group.readPathEntry( "InfoFilename","kdebug.dbg" ) );
    //pInfoShow->setText( group.readEntry( "InfoShow" ) );
    pWarnCombo->setCurrentIndex( group.readEntry( "WarnOutput", 2 ) );
    pWarnFile->setText( group.readPathEntry( "WarnFilename","kdebug.dbg" ) );
    //pWarnShow->setText( group.readEntry( "WarnShow" ) );
    pErrorCombo->setCurrentIndex( group.readEntry( "ErrorOutput", 2 ) );
    pErrorFile->setText( group.readPathEntry( "ErrorFilename","kdebug.dbg") );
    //pErrorShow->setText( group.readEntry( "ErrorShow" ) );
    pFatalCombo->setCurrentIndex( group.readEntry( "FatalOutput", 2 ) );
    pFatalFile->setText( group.readPathEntry("FatalFilename","kdebug.dbg") );
    //pFatalShow->setText( group.readEntry( "FatalShow" ) );
    pAbortFatal->setChecked( group.readEntry( "AbortFatal", 1 ) );
    slotDestinationChanged();
}

void KCMDebug::slotDisableAllChanged()
{
    kDebug();
    bool enabled = !m_disableAll->isChecked();
    m_areaWidget->setEnabled(enabled);
    pInfoGroup->setEnabled(enabled);
    pWarnGroup->setEnabled(enabled);
    pErrorGroup->setEnabled(enabled);
    pFatalGroup->setEnabled(enabled);
    pAbortFatal->setEnabled(enabled);

    KConfigGroup topGroup(pConfig, QString());
    if (m_disableAll->checkState() == topGroup.readEntry("DisableAll", false)) {
        emit changed( false );
    } else {
        emit changed( true );
    }
}


void KCMDebug::slotDebugAreaChanged(QTreeWidgetItem* item)
{
    // Save settings from previous page
    save();

    const QString areaName = item->data(0, Qt::UserRole).toString();
    showArea(areaName);
}

void KCMDebug::slotDestinationChanged()
{
    pInfoFile->setEnabled(pInfoCombo->currentIndex() == 0);
    pWarnFile->setEnabled(pWarnCombo->currentIndex() == 0);
    pErrorFile->setEnabled(pErrorCombo->currentIndex() == 0);
    pFatalFile->setEnabled(pFatalCombo->currentIndex() == 0);
}

void KCMDebug::slotAbortFatalChanged()
{
    save();
}
#include "moc_kdebugconfig.cpp"
