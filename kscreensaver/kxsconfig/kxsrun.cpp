//-----------------------------------------------------------------------------
//
// KDE xscreensaver launcher
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
#include <config-kxsconfig.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <qfile.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "kxsitem.h"
#include "kxsxml.h"

#define MAX_ARGS  30

//===========================================================================
static const char appName[] = "kxsrun";

static const char description[] = I18N_NOOP("KDE X Screen Saver Launcher");

static const char version[] = "3.0.0";

int main(int argc, char *argv[])
{
    KLocale::setMainCatalog("kxsconfig");
    KCmdLineArgs::init(argc, argv, appName, 0, ki18n("KXSRun"), version, ki18n(description));


    KCmdLineOptions options;

    options.add("+screensaver", ki18n("Filename of the screen saver to start"));

    options.add("+-- [options]", ki18n("Extra options to pass to the screen saver"));

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app( false );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if ( !args->count() )
	exit( 1 );

    QString filename = args->arg(0);
    QString configFile(filename);

    // Get the config filename
    int slash = filename.lastIndexOf(QLatin1Char( '/' ));
    if (slash >= 0)
	configFile = filename.mid(slash+1);

    QString exeName = configFile;
    configFile += QLatin1String( "rc" );

    // read configuration args
    KConfig config(configFile);

    QList<KXSConfigItem*> configItemList;

    QString xmlFile = QLatin1String( "/doesntexist" );
#ifdef XSCREENSAVER_CONFIG_DIR
    xmlFile = QLatin1String( XSCREENSAVER_CONFIG_DIR );
#endif
    xmlFile += QLatin1Char( '/' ) + exeName + QLatin1String( ".xml" );
    if ( QFile::exists( xmlFile ) ) {
	// We can use the xscreensaver xml config files.
	KXSXml xmlParser(0);
	xmlParser.parse(xmlFile);
	configItemList = xmlParser.items();
        for ( int i = 0; i < configItemList.size(); i++ ) {
	    configItemList[i]->read( config );
	}
    } else {
	// fall back to KDE's old config files.
	int idx = 0;
	while (true)
	{
	    QString group = QString(QLatin1String( "Arg%1" )).arg(idx);
	    if (config.hasGroup(group)) {
                KConfigGroup grp = config.group(group);
		QString type = grp.readEntry("Type");
		if (type == QLatin1String( "Range" )) {
		    KXSRangeItem *rc = new KXSRangeItem(group, config);
		    configItemList.append(rc);
		} else if (type == QLatin1String( "DoubleRange" )) {
		    KXSDoubleRangeItem *rc = new KXSDoubleRangeItem(group, config);
		    configItemList.append(rc);
		} else if (type == QLatin1String( "Check" )) {
		    KXSBoolItem *cc = new KXSBoolItem(group, config);
		    configItemList.append(cc);
		} else if (type == QLatin1String( "DropList" )) {
		    KXSSelectItem *si = new KXSSelectItem(group, config);
		    configItemList.append(si);
		}
	    } else {
		break;
	    }
	    idx++;
	}
    }

    // find the xscreensaver executable
    //work around a KStandarDirs::findExe() "feature" where it looks in $KDEDIR/bin first no matter what and sometimes finds the wrong executable
    QFileInfo checkExe;
    QString saverdir = QString(QLatin1String( "%1/%2" )).arg(QLatin1String( XSCREENSAVER_HACKS_DIR )).arg(filename);
    kDebug() << "saverdir is" << saverdir;
    QByteArray exeFile;
    checkExe.setFile(saverdir);
    if (checkExe.exists() && checkExe.isExecutable() && checkExe.isFile())
    {
        exeFile = QFile::encodeName(saverdir);
    }


    if (!exeFile.isEmpty()) {
	char *sargs[MAX_ARGS];

        {
            QByteArray encodedFile = QFile::encodeName(filename);
            sargs[0] = new char [encodedFile.size()+1];
            strcpy(sargs[0], encodedFile.constData());
        }

	// add the command line options
	QString cmd;
	int i;
	for (i = 1; i < args->count(); i++)
	    cmd += QLatin1Char( ' ' ) + QString(args->arg(i));

	// add the config options
        for ( int i = 0; i < configItemList.size(); i++ ) {
	    cmd += QLatin1Char( ' ' ) + configItemList[i]->command();
	}

	// put into char * array for execv
	QString word;
	int si = 1;
	i = 0;
	bool inQuotes = false;
	while (i < cmd.length() && si < MAX_ARGS-1) {
	    word = QLatin1String( "" );
	    while ( cmd[i].isSpace() && i < cmd.length() ) i++;
	    while ( (!cmd[i].isSpace() || inQuotes) && i < cmd.length() ) {
		if ( cmd[i] == QLatin1Char( '\"' ) ) {
		    inQuotes = !inQuotes;
		} else {
		    word += cmd[i];
		}
		i++;
	    }
	    if (!word.isEmpty()) {
                // filenames are likely to be the troublesome encodings
                QByteArray encodedWord = QFile::encodeName(word);
		sargs[si] = new char [encodedWord.size()+1];
		strcpy(sargs[si], encodedWord.constData());
		si++;
	    }
	}

	sargs[si] = 0;

	// here goes...
	execv(exeFile.constData(), sargs);
    }
}


