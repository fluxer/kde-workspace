//-----------------------------------------------------------------------------
//
// Screen savers for KDE
//
// Copyright (c)  Martin R. Jones 1999
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <qcolor.h>

#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kcrash.h>
#include <QDesktopWidget>
#include "demowin.h"
#include "saver.h"

static const char appName[] = "klock";
static const char description[] = I18N_NOOP("KDE Screen Lock/Saver");
static const char version[] = "2.0.0";

static void crashHandler( int /*sig*/ )
{
#ifdef SIGABRT
    signal ( SIGABRT, SIG_DFL );
#endif
    abort();
}

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    KCmdLineArgs::init(argc, argv, appName, 0, ki18n("KLock"), version, ki18n(description));


    KCmdLineOptions options;

    options.add("setup", ki18n("Setup screen saver"));

    options.add("window-id wid", ki18n("Run in the specified XWindow"));

    options.add("root", ki18n("Run in the root XWindow"));

    options.add("demo", ki18n("Start screen saver in demo mode"), "default");

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    KCrash::setCrashHandler( crashHandler );

    DemoWindow *demoWidget = 0;
    Window saveWin = 0;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->isSet("setup"))
    {
       setupScreenSaver();
       exit(0);
    }

    if (args->isSet("window-id"))
    {
        saveWin = args->getOption("window-id").toInt();
    }

    if (args->isSet("root"))
    {
        saveWin = QApplication::desktop()->handle();
    }

    if (args->isSet("demo"))
    {
        saveWin = 0;
    }

    if (saveWin == 0)
    {
        demoWidget = new DemoWindow();
        demoWidget->setAttribute(Qt::WA_NoSystemBackground, true);
        demoWidget->show();
        saveWin = demoWidget->winId();
        app.processEvents();
    }

    startScreenSaver(saveWin);
    app.exec();
    stopScreenSaver();

    if (demoWidget)
    {
        delete demoWidget;
    }

    return 0;
}

