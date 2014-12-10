/*
 * Copyright (C) 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <qwidget.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qstring.h>
#include <q3valuelist.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QList>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kparts/part.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kmimetype.h>
#include <kmimetypetrader.h>

#include <kscreensaver.h>

#include "kpartsaver.h"
#include <kaboutapplicationdialog.h>
#include <kiconloader.h>
#include <kglobal.h>


QList<KPartSaver*> g_savers;
bool g_inited = false;

static KAboutData* s_aboutData = 0;
class KPartSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return s_aboutData;
    }


    virtual KScreenSaver* create( WId d )
    {
        KGlobal::locale()->insertCatalog(QLatin1String( "kpartsaver" ));
        return new KPartSaver( d );
    }

    virtual QDialog* setup()
    {
        KGlobal::locale()->insertCatalog(QLatin1String( "kpartsaver" ));
        return new SaverConfig;
    }
};

int main( int argc, char *argv[] )
{
    s_aboutData = new KAboutData( "kpartsaver", 0, ki18n( "KPart Screen Saver" ), "1.0", ki18n( "KPart Screen Saver" ) );

    KPartSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}


void exitHandler( int )
{
    kDebug() << "exitHandler";
    qDeleteAll(g_savers);
    //KLibLoader::self()->cleanUp();
    exit(0);
}


KPartSaver::KPartSaver( WId id )
    : KScreenSaver( id ), m_timer(), m_part(0), m_current(-1), m_back(0)
{
    // install signal handlers to make sure that nspluginviewer is shutdown correctly
    // move this into the nspluginviewer kpart code
    if( !g_inited ) {
        g_inited = true;

        srand( time(0) );

        // install signal handler
        signal( SIGINT, exitHandler );    // Ctrl-C will cause a clean exit...
        signal( SIGTERM, exitHandler );   // "kill"...
        signal( SIGHUP, exitHandler );    // "kill -HUP" (hangup)...
        signal( SIGKILL, exitHandler );    // "kill -KILL"
        //atexit( ( void (*)(void) ) exitHandler );
    }

    g_savers.append( this );

    closeUrl();

    // load config
    KConfigGroup cfg(KGlobal::config(), "Misc");

    m_single = cfg.readEntry( "Single", true );
    m_delay = cfg.readEntry( "Delay", 60 );
    m_random = cfg.readEntry( "Random", false );
    m_files = cfg.readEntry( "Files",QStringList() );

    if( m_files.count()==0 ) {

        // create background widget
        m_back = new QLabel( i18n("The screen saver is not configured yet."), this );
        m_back->setWordWrap( true );
        m_back->setAlignment( Qt::AlignCenter );
        embed( m_back );
        m_back->show();

    } else {

        // queue files
        for( int n=0; n<m_files.count(); n++ )
            queue( KUrl( m_files[n] ) );

        // play files
        if( m_single )
            next( m_random );
        else {
            next( m_random );
            QTimer::singleShot( m_delay*1000, this, SLOT(timeout()) );
        }
    }
}


KPartSaver::~KPartSaver()
{
    g_savers.removeAll( this );
    closeUrl();
}


void KPartSaver::closeUrl()
{
    if( m_part ) {
        m_part->closeUrl();
        delete m_part;
        m_part = 0;
    }
}


bool KPartSaver::openUrl( const KUrl &url )
{
    if( m_part )
        closeUrl();

    // find mime type
    QString mime = KMimeType::findByUrl( url )->name();

    // load part
    m_part = KMimeTypeTrader::createPartInstanceFromQuery<KParts::ReadOnlyPart>(
        mime, this, this, QString() );

    if( !m_part ) {
        kDebug() << "Part for " << url << " can't be constructed";
        closeUrl();
        return false;
    } else
        embed( m_part->widget() );

    // show kpart
    delete m_back;
    m_back = 0;

    show();
    m_part->widget()->show();

    // load url
    if( !m_part->openUrl( url ) ) {
        kDebug() << "Can't load " << url.url();
        delete m_part;
        m_part = 0;
        closeUrl();
        return false;
    }

    return true;
}


void KPartSaver::queue( const KUrl &url )
{
    Medium medium;
    medium.url = url;
    medium.failed = false;
    m_media.append( medium );
}


void KPartSaver::timeout()
{
    next( m_random );
    QTimer::singleShot( m_delay*1000, this, SLOT(timeout()) );
}


void KPartSaver::next( bool random )
{
    // try to find working media
    while( m_media.count()>0 ) {

        if( random )
            m_current = rand() % m_media.count();
        else
            m_current++;

        if( m_current>=(int)m_media.count() )
            m_current = 0;

        kDebug() << "Trying medium " << m_media[m_current].url.url();

        // either start immediately or start mimejob first
        if( !openUrl( m_media[m_current].url ) ) {
            m_media.removeAll( m_media.at(m_current) );
            m_current--;
        } else
            return;

    }

    // create background widget
    m_back = new QLabel( i18n("All of your files are unsupported"), this );
    m_back->setWordWrap( true );
    m_back->setAlignment( Qt::AlignCenter );
    embed( m_back );
    m_back->show();

    // nothing found, set to invalid
    m_current = -1;
}


/*******************************************************************************/


SaverConfig::SaverConfig( QWidget* parent )
    : KDialog( parent )
{
    setCaption(i18n( "Media Screen Saver" ));
    setButtons(Ok|Cancel|Help);
    setDefaultButton(Ok);
    setModal(true);
    setButtonText( Help, i18n( "A&bout" ) );
    QWidget *main = new QWidget(this);
    setMainWidget(main);
    cfg = new ConfigWidget();
    cfg->setupUi( main );

    connect(this,SIGNAL(okClicked()),this,SLOT(apply()));
    connect(this,SIGNAL(helpClicked()),this,SLOT(slotHelp()));

    connect( cfg->m_multiple, SIGNAL(toggled(bool)), cfg->m_delayLabel, SLOT(setEnabled(bool)) );
    connect( cfg->m_multiple, SIGNAL(toggled(bool)), cfg->m_delay, SLOT(setEnabled(bool)) );
    connect( cfg->m_multiple, SIGNAL(toggled(bool)), cfg->m_secondsLabel, SLOT(setEnabled(bool)) );
    connect( cfg->m_multiple, SIGNAL(toggled(bool)), cfg->m_random, SLOT(setEnabled(bool)) );

    connect( cfg->m_files, SIGNAL(itemSelectionChanged()), SLOT(select()) );
    connect( cfg->m_add, SIGNAL(clicked()), SLOT(add()) );
    connect( cfg->m_remove, SIGNAL(clicked()), SLOT(remove()) );
    connect( cfg->m_up, SIGNAL(clicked()), SLOT(up()) );
    connect( cfg->m_down, SIGNAL(clicked()), SLOT(down()) );

    cfg->m_up->setIcon( KIcon(QLatin1String( "go-up" )) );
    cfg->m_down->setIcon( KIcon(QLatin1String( "go-down" )) );

    // load config
    KConfigGroup grp(KGlobal::config(), "Misc");

    bool single = grp.readEntry( "Single", true );
    cfg->m_single->setChecked( single );
    cfg->m_multiple->setChecked( !single );
    cfg->m_delay->setMinimum( 1 );
    cfg->m_delay->setMaximum( 10000 );
    cfg->m_delay->setValue( grp.readEntry( "Delay", 60 ) );
    cfg->m_random->setChecked( grp.readEntry( "Random", false ) );
    cfg->m_files->addItems( grp.readEntry( "Files",QStringList() ) );

    // update buttons
    select();
}


SaverConfig::~SaverConfig()
{
    delete cfg;
}


void SaverConfig::apply()
{
    kDebug() << "apply";

    KConfigGroup grp(KGlobal::config(), "Misc");

    grp.writeEntry( "Single", cfg->m_single->isChecked() );
    grp.writeEntry( "Delay", cfg->m_delay->value() );
    grp.writeEntry( "Random", cfg->m_random->isChecked() );

    int num = cfg->m_files->count();
    QStringList files;
    for( int n=0; n<num; n++ )
        files << cfg->m_files->item(n)->text();

    grp.writeEntry( "Files", files );

    grp.sync();
    accept();
}


void SaverConfig::add()
{
    const KUrl::List files = KFileDialog::getOpenUrls( KUrl(), QString(),
                                                 this, i18n("Select Media Files") );
    for( int n=0; n<files.count(); n++ )
        cfg->m_files->addItem( files[n].prettyUrl() );
}


void SaverConfig::remove()
{
    int current = cfg->m_files->currentRow();
    if( current!=-1 )
        cfg->m_files->takeItem( current );
}


void SaverConfig::select()
{
    bool enabled = cfg->m_files->currentRow()!=-1;
    cfg->m_remove->setEnabled( enabled );
    cfg->m_up->setEnabled( enabled && cfg->m_files->currentRow()!=0 );
    cfg->m_down->setEnabled( enabled && cfg->m_files->currentRow()!=(int)cfg->m_files->count()-1 );
}


void SaverConfig::up()
{
    const int current = cfg->m_files->currentRow();
    if ( current>0 ) {
        QString txt = cfg->m_files->currentItem()->text();
        cfg->m_files->takeItem( current );
        cfg->m_files->insertItem( current-1, txt );
        cfg->m_files->setCurrentRow( current-1 );
    }
}


void SaverConfig::down()
{
    const int current = cfg->m_files->currentRow();
    if ( current!=-1 && current<(int)cfg->m_files->count()-1 ) {
        QString txt = cfg->m_files->currentItem()->text();
        cfg->m_files->takeItem( current );
        cfg->m_files->insertItem( current+1, txt );
        cfg->m_files->setCurrentRow( current+1 );
    }
}

void SaverConfig::slotHelp()
{
    KAboutApplicationDialog mAbout(s_aboutData, this);
    mAbout.exec();
}

#include "kpartsaver.moc"
