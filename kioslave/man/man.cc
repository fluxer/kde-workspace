#include "man.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h> // getpid()

#include <QFile>

#include <kdebug.h>
#include <kshell.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <kdemacros.h>

using namespace KIO;

ManProtocol::ManProtocol( const QByteArray &pool, const QByteArray &app )
    : SlaveBase( "man", pool, app )
    , m_page( "" )
{
    kDebug( 7107 ) << "ManProtocol::ManProtocol";
    m_bash = KGlobal::dirs()->findExe( "bash" );
    m_manScript = KStandardDirs::locate( "data", "kio_man/kde-man2html" );

    if( m_bash.isNull() || m_manScript.isNull()) {
        kError( 7107 ) << "Critical error: Cannot locate files for HTML-conversion" << endl;
        QString errorStr;
        if ( m_bash.isNull() ) {
            errorStr = "bash.";
        } else {
            QString missing =m_manScript.isNull() ?  "kio_man/kde-man2html" : "kio_man/kde-man2html.conf";
            errorStr = "kde-man2html" + i18n( "\nUnable to locate file %1 which is necessary to run this service. "
                "Please check your software installation." ,  missing );
        }
        error( KIO::ERR_CANNOT_LAUNCH_PROCESS, errorStr );
        exit();
    }

    kDebug( 7107 ) << "ManProtocol::ManProtocol - done";
}

ManProtocol::~ManProtocol()
{
    kDebug( 7107 ) << "ManProtocol::~ManProtocol";
    kDebug( 7107 ) << "ManProtocol::~ManProtocol - done";
}

void ManProtocol::get( const KUrl& url )
{
    kDebug( 7107 ) << "ManProtocol::get";
    kDebug( 7107 ) << "URL: " << url.prettyUrl() << " , Path :" << url.path();

    if (url.path()=="/")
    {
       KUrl newUrl("man:/");
       redirection(newUrl);
       finished();
       return;
    };

    // some people write man://autoconf instead of man:/autoconf
    if (!url.host().isEmpty()) {
        KUrl newURl(url);
        newURl.setPath(url.host()+url.path());
        newURl.setHost(QString());
        redirection(newURl);
        finished();
        return;
    }

    if ( url.path().right(1) == "/" )
    {
        // Trailing / are not supported, so we need to remove them.
        KUrl newUrl( url );
        QString newPath( url.path() );
        newPath.chop( 1 );
        newUrl.setPath( newPath );
        redirection( newUrl );
        finished();
        return;
    }

    // '<' in the path looks suspicious, someone is trying info:/dir/<script>alert('xss')</script>
    if (url.path().contains('<'))
    {
        error(KIO::ERR_DOES_NOT_EXIST, url.url());
        return;
    }

    mimeType("text/html");
    // extract the path and node from url
    decodeURL( url );

    QString cmd = KShell::quoteArg(m_bash);
    cmd += ' ';
    cmd += KShell::quoteArg(m_manScript);
    cmd += ' ';
    cmd += KShell::quoteArg(m_page);

    kDebug( 7107 ) << "cmd: " << cmd;

    FILE *file = popen( QFile::encodeName(cmd), "r" );
    if ( !file ) {
        kDebug( 7107 ) << "ManProtocol::get popen failed";
        error( ERR_CANNOT_LAUNCH_PROCESS, cmd );
        return;
    }

    char buffer[ 4096 ];

    bool empty = true;
    while ( !feof( file ) )
    {
      int n = fread( buffer, 1, sizeof( buffer ), file );
      if ( !n && feof( file ) && empty ) {
        error( ERR_CANNOT_LAUNCH_PROCESS, cmd );
        return;
      }
      if ( n < 0 )
      {
        kDebug( 7107 ) << "ManProtocol::get ERROR!";
        pclose( file );
        return;
      }

      empty = false;
      data( QByteArray::fromRawData( buffer, n ) );
    }

    pclose( file );

    finished();

    kDebug( 7107 ) << "ManProtocol::get - done";
}

void ManProtocol::mimetype( const KUrl& /* url */ )
{
    kDebug( 7107 ) << "ManProtocol::mimetype";

    // to get rid of those "Open with" dialogs...
    mimeType( "text/html" );

    // finish action
    finished();

    kDebug( 7107 ) << "ManProtocol::mimetype - done";
}

void ManProtocol::decodeURL( const KUrl &url )
{
    kDebug( 7107 ) << "ManProtocol::decodeURL";

    decodePath( url.path() );

    kDebug( 7107 ) << "ManProtocol::decodeURL - done";
}

void ManProtocol::decodePath( QString path )
{
    kDebug( 7107 ) << "ManProtocol::decodePath(-" <<path<<"-)";

    m_page = "man";  // default

    // remove leading slash
    if ('/' == path[0]) {
      path = path.mid( 1 );
    }

    // remove trailing section and compression
    path.replace(QRegExp(".[1-9](.gz|.bz2|.xz)?$"), "");

    kDebug( 7107 ) << "Path: " << path;

    int slashPos = path.indexOf( "/" );

    if( slashPos < 0 )
    {
        m_page = path;
        return;
    }

    m_page = path.left( slashPos );

    kDebug( 7107 ) << "ManProtocol::decodePath - done";
}

// A minimalistic stat with only the file type
// This seems to be enough for konqueror
void ManProtocol::stat( const KUrl & )
{
	UDSEntry uds_entry;

	// Regular file with rwx permission for all
	uds_entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO );

	statEntry( uds_entry );

	finished();
}

extern "C" { int KDE_EXPORT kdemain( int argc, char **argv ); }

int kdemain( int argc, char **argv )
{
  KComponentData componentData( "kio_man" );

  kDebug() << "kio_man starting " << getpid();

  if (argc != 4)
  {
     fprintf(stderr, "Usage: kio_man protocol domain-socket1 domain-socket2\n");
     exit(-1);
  }

  ManProtocol slave( argv[2], argv[3] );
  slave.dispatchLoop();

  return 0;
}
