//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
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

// This file contains code copied from xscreensaver.  The xscreensaver
// copyright notice follows.

/*
 * xscreensaver, Copyright (c) 1993-2002 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 */
#include <config-kxsconfig.h>

#include <stdlib.h>
#include <qlayout.h>
#include <qtimer.h>
#include <kvbox.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kshell.h>
#include <kmessagebox.h>

#include "kxsconfig.h"
#include "kxscontrol.h"
#include "kxsxml.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>

int ignoreXError(Display *, XErrorEvent *);

//===========================================================================

const uint widgetEventMask =                 // X event mask
(uint)(
       ExposureMask |
       PropertyChangeMask |
       StructureNotifyMask
      );

KXSConfigDialog::KXSConfigDialog(const QString &filename, const QString &name)
  : KDialog(0),
    mFilename(filename), mPreviewProc(0), mKilled(false)
{
    Q_UNUSED(name)

    setButtons(Ok| Cancel);
    setDefaultButton( Ok);
    setModal(false);
    int slash = filename.lastIndexOf(QLatin1Char( '/' ));
    if (slash >= 0)
	mConfigFile = filename.mid(slash+1);
    else
	mConfigFile = filename;

    mExeName = mConfigFile;
    mConfigFile += QLatin1String( "rc" );
    connect( this,SIGNAL(okClicked()),this,SLOT(slotOk()));
    connect( this,SIGNAL(cancelClicked()),this,SLOT(slotCancel()));
}

bool KXSConfigDialog::create()
{
    QWidget *main = new QWidget(this);
    setMainWidget(main);
    QVBoxLayout *topLayout = new QVBoxLayout(main);
    topLayout->setSpacing(spacingHint());
    QHBoxLayout *layout = new QHBoxLayout();
    topLayout->addLayout(layout);
    layout->setSpacing(spacingHint());
    KVBox *controlLayout = new KVBox(main);
    controlLayout->setSpacing(spacingHint());
    layout->addWidget(controlLayout);
    ((QBoxLayout*)controlLayout->layout())->addStrut(120);

    KConfig config(mConfigFile);

    QString xmlFile = QLatin1String( "/doesntexist" );
#ifdef XSCREENSAVER_CONFIG_DIR
    xmlFile =QLatin1String( XSCREENSAVER_CONFIG_DIR );
#endif

    xmlFile += QLatin1Char( '/' ) + mExeName + QLatin1String( ".xml" );
    if ( QFile::exists( xmlFile ) ) {
	// We can use the xscreensaver xml config files.
	KXSXml xmlParser(controlLayout);
	xmlParser.parse( xmlFile );
	mConfigItemList = xmlParser.items();
	QWidget *spacer = new QWidget(controlLayout);
	controlLayout->setStretchFactor(spacer, 1 );
	QString name = xmlParser.name();
	setPlainCaption(i18n("Setup %1", i18nc("@item screen saver name", name.toUtf8())));
	QString descr = xmlParser.description();
	if ( !descr.isEmpty() ) {
	    // NOTE: Simplification must be same as in PO template creation script.
	    descr = descr.replace(QLatin1Char( '\n' ), QLatin1Char( ' ' )).simplified();
	    // NOTE: Contexts has to match with that extracted by the PO template creation script.
	    QLabel *l = new QLabel( i18nc( "@info screen saver description", descr.toUtf8() ), main );
	    l->setWordWrap( 1 );
	    topLayout->addWidget( l );
	}
    } else {
        // fall back to KDE's old config files.
	int idx = 0;
	while (true) {
	    QString group = QString(QLatin1String( "Arg%1" )).arg(idx);
	    if (config.hasGroup(group)) {
		KConfigGroup grp = config.group(group);
		QString type = grp.readEntry("Type");
		if (type == QLatin1String( "Range" )) {
		    KXSRangeControl *rc = new KXSRangeControl(controlLayout, group, config);
		    mConfigItemList.append(rc);
		} else if (type == QLatin1String( "DoubleRange" )) {
		    KXSDoubleRangeControl *rc =
			new KXSDoubleRangeControl(controlLayout, group, config);
		    mConfigItemList.append(rc);
		} else if (type == QLatin1String( "Check" )) {
		    KXSCheckBoxControl *cc = new KXSCheckBoxControl(controlLayout, group,
			    config);
		    mConfigItemList.append(cc);
		} else if (type == QLatin1String( "DropList" )) {
		    KXSDropListControl *dl = new KXSDropListControl(controlLayout, group,
			    config);
		    mConfigItemList.append(dl);
		}
	    } else {
		break;
	    }
	    idx++;
	}
	if ( idx == 0 )
	    return false;
    }

    for ( int i = 0; i < mConfigItemList.size(); i++ ) {
        KXSConfigItem *item = mConfigItemList[i];
        item->read( config );
        QWidget *widget = dynamic_cast<QWidget*>( item );
        if ( widget ) {
            connect( widget, SIGNAL(changed()), SLOT(slotChanged()) );
        }
    }

    mPreviewProc = new KProcess;
    connect(mPreviewProc, SIGNAL(finished(int,QProcess::ExitStatus)),
	    SLOT(slotPreviewProcFinished(int,QProcess::ExitStatus)));

    mPreviewTimer = new QTimer(this);
    mPreviewTimer->setSingleShot(true);
    connect(mPreviewTimer, SIGNAL(timeout()), SLOT(slotNewPreview()));

    mPreview = new QWidget(main);
    mPreview->setFixedSize(250, 200);
    {
        QPalette palette;
        palette.setColor(mPreview->backgroundRole(), Qt::black);
        mPreview->setPalette(palette);
	mPreview->setAutoFillBackground(true);
    }

    layout->addWidget(mPreview);
    show();

    // So that hacks can XSelectInput ButtonPressMask
    XSelectInput(QX11Info::display(), mPreview->winId(), widgetEventMask );

    startProcess();
    return true;
}

//---------------------------------------------------------------------------
KXSConfigDialog::~KXSConfigDialog()
{
  if (mPreviewProc && mPreviewProc->state() == QProcess::Running) {
    mPreviewProc->kill();
    mPreviewProc->waitForFinished(5000);
    delete mPreviewProc;
  }
}

//---------------------------------------------------------------------------
QString KXSConfigDialog::command()
{
  QString cmd;

  for (int i = 0; i < mConfigItemList.size(); i++)
  {
    if (mConfigItemList[i]) {
      cmd += QLatin1String( " " ) + mConfigItemList[i]->command();
    }
  }

  return cmd;
}

//---------------------------------------------------------------------------
void KXSConfigDialog::startProcess()
{
    mKilled = false;
    mPreviewProc->clearProgram();
    QString saver = QString( QLatin1String( "%1 -window-id 0x%2" ) )
        .arg( mFilename )
        .arg( long(mPreview->winId()), 0, 16 );
    saver += command();
    saver = saver.trimmed();

    kDebug() << "Command: " <<  saver;

    int i = 0;
    QString exe;
    while ( !saver[i].isSpace() ) {
        exe += saver[i++];
    }
    // work around a KStandarDirs::findExe() "feature" where it looks in
    // $KDEDIR/bin first no matter what and sometimes finds the wrong executable
    QFileInfo checkExe;
    QString saverdir = QLatin1String(XSCREENSAVER_HACKS_DIR  "/" ) + exe;
    QString path;
    checkExe.setFile(saverdir);
    if (checkExe.exists() && checkExe.isExecutable() && checkExe.isFile())
    {
        path = saverdir;
    }
    if (!path.isEmpty()) {
        (*mPreviewProc) << path;

        (*mPreviewProc) << KShell::splitArgs(saver.mid(i));

        mPreviewProc->start();
    }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotPreviewProcFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    if ( mKilled ) {
        startProcess();
    } else {
	// stops us from spawning the hack really fast, but still not the best
	QString path = KStandardDirs::findExe(mFilename,QLatin1String( XSCREENSAVER_HACKS_DIR ));
	if ( QFile::exists(path) ) {
	    mKilled = true;
	    slotChanged();
	}
    }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotNewPreview()
{
  if (mPreviewProc->state() == QProcess::Running) {
    mKilled = true;
    mPreviewProc->kill(); // restarted in slotPreviewExited()
  } else {
    startProcess();
  }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotChanged()
{
    mPreviewTimer->start(1000);
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotOk()
{
  KConfig config(mConfigFile);

  for (int i = 0; i < mConfigItemList.size(); i++)
  {
    if (mConfigItemList[i]) {
      mConfigItemList[i]->save(config);
    }
  }

  kapp->quit();
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotCancel()
{
  kapp->quit();
}


//===========================================================================

static const char appName[] = "kxsconfig";

static const char description[] = I18N_NOOP("KDE X Screen Saver Configuration tool");

static const char version[] = "3.0.0";

static const char *defaults[] = {
#include "XScreenSaver_ad.h"
 0
};

const char *progname = 0;
const char *progclass = "XScreenSaver";
XrmDatabase db;

int main(int argc, char *argv[])
{
  KCmdLineArgs::init(argc, argv, appName, 0, ki18n("KXSConfig"), version, ki18n(description));


  KCmdLineOptions options;

  options.add("+screensaver", ki18n("Filename of the screen saver to configure"));

  options.add("+[savername]", ki18n("Optional screen saver name used in messages"));

  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;
  KGlobal::locale()->insertCatalog(QLatin1String( "kxsconfig" ));

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if(args->count()==0)
    exit(1);

  /* We must read exactly the same resources as xscreensaver.
     That means we must have both the same progclass *and* progname,
     at least as far as the resource database is concerned.  So,
     put "xscreensaver" in argv[0] while initializing Xt.
   */
  const char *dummyargs[] = { "xscreensaver" };
  int dummyargc = 1;
  progname = dummyargs[0];

  // Teach Xt to use the Display that Qt has already opened.
  XtToolkitInitialize ();
  XtAppContext xtApp = XtCreateApplicationContext ();
  Display *dpy = QX11Info::display();
  XtAppSetFallbackResources (xtApp, const_cast<char**>(defaults));
  XtDisplayInitialize (xtApp, dpy, progname, progclass, 0, 0,
                       &dummyargc,
                       const_cast<char**>(dummyargs));
  Widget toplevel_shell = XtAppCreateShell (progname, progclass,
	  applicationShellWidgetClass,
	  dpy, 0, 0);
  dpy = XtDisplay (toplevel_shell);
  db = XtDatabase (dpy);
  XtGetApplicationNameAndClass (dpy, const_cast<char**>(&progname),
                                const_cast<char**>(&progclass));

  QString name = args->arg(args->count() - 1);
  KXSConfigDialog *dialog=new KXSConfigDialog(args->arg(0), name);
  if ( dialog->create() ) {
      dialog->show();
      app.exec();
  } else {
      KMessageBox::sorry(0,
	      i18n("No configuration available for %1", name),
	      name );
  }

  delete dialog;
}

#include "kxsconfig.moc"
