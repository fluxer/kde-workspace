/*****************************************************************
ksmserver - the KDE session management server

Copyright 2000 Matthias Ettrich <ettrich@kde.org>
Copyright 2005 Lubos Lunak <l.lunak@kde.org>

relatively small extensions by Oswald Buddenhagen <ob6@inf.tu-dresden.de>

some code taken from the dcopserver (part of the KDE libraries), which is
Copyright 1999 Matthias Ettrich <ettrich@kde.org>
Copyright 1999 Preston Brown <pbrown@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "config-unix.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include <QPushButton>
#include <QTimer>
#include <QtDBus/QtDBus>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <ktemporaryfile.h>
#include <knotification.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include "global.h"
#include "server.h"
#include "client.h"

#include <QtGui/qx11info_x11.h>

#include "kcminit_interface.h"

//#define KSMSERVER_STARTUP_DEBUG1

#ifdef KSMSERVER_STARTUP_DEBUG1
static QTime t;
#endif

/*!  Restores the previous session. Ensures the window manager is
  running (if specified).
 */
void KSMServer::restoreSession( const QString &sessionName )
{
    if( state != Idle )
        return;
#ifdef KSMSERVER_STARTUP_DEBUG1
    t.start();
#endif
    state = LaunchingWM;

    kDebug() << "KSMServer::restoreSession " << sessionName;
    KSharedConfig::Ptr config = KGlobal::config();

    sessionGroup = "Session: " + sessionName;
    KConfigGroup configSessionGroup( config, sessionGroup);

    int count =  configSessionGroup.readEntry( "count", 0 );
    appsToStart = count;
    connect( klauncherSignals, SIGNAL(autoStart0Done()), SLOT(autoStart0Done()));
    connect( klauncherSignals, SIGNAL(autoStart1Done()), SLOT(autoStart1Done()));
    connect( klauncherSignals, SIGNAL(autoStart2Done()), SLOT(autoStart2Done()));

    // find all commands to launch the wm in the session
    QList<QStringList> wmStartCommands;
    if ( !wm.isEmpty() ) {
        for ( int i = 1; i <= count; i++ ) {
            QString n = QString::number(i);
            if ( isWM( configSessionGroup.readEntry( QString("program")+n, QString() ) ) ) {
                wmStartCommands << configSessionGroup.readEntry( QString("restartCommand")+n, QStringList() );
            }
        }
    } 
    if( wmStartCommands.isEmpty()) // otherwise use the configured default
        wmStartCommands << wmCommands;

    launchWM( wmStartCommands );
}

/*!
  Starts the default session.

  Currently, that's the window manager only (if specified).
 */
void KSMServer::startDefaultSession()
{
    if( state != Idle )
        return;
    state = LaunchingWM;
#ifdef KSMSERVER_STARTUP_DEBUG1
    t.start();
#endif
    sessionGroup = "";
    connect( klauncherSignals, SIGNAL(autoStart0Done()), SLOT(autoStart0Done()));
    connect( klauncherSignals, SIGNAL(autoStart1Done()), SLOT(autoStart1Done()));
    connect( klauncherSignals, SIGNAL(autoStart2Done()), SLOT(autoStart2Done()));

    launchWM( QList< QStringList >() << wmCommands );
}

void KSMServer::launchWM( const QList< QStringList >& wmStartCommands )
{
    assert( state == LaunchingWM );

    // when we have a window manager, we start it first and give
    // it some time before launching other processes. Results in a
    // visually more appealing startup.
    QStringList wmCommand = wmStartCommands[0];
    QString program = wmCommand.takeAt(0);
    connect( wmProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(wmProcessChange()));
    connect( wmProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(wmProcessChange()));
    wmProcess->start(program, wmCommand);
    wmProcess->waitForStarted(4000);
    QMetaObject::invokeMethod(this, "autoStart0", Qt::QueuedConnection);
}

void KSMServer::clientSetProgram( KSMClient* client )
{
    if( client->program() == wm )
        autoStart0();
}

void KSMServer::wmProcessChange()
{
    if( state != LaunchingWM ) {
        // don't care about the process when not in the wm-launching state anymore
        disconnect(wmProcess, 0, this, 0);
        return;
    }
    if( wmProcess->state() == QProcess::NotRunning ) {
        // wm failed to launch for some reason, go with kwin instead
        kWarning() << "Window manager" << wm << "failed to launch";
        if( wm == "kwin" )
            return; // uhoh, kwin itself failed
        kDebug() << "Launching KWin";
        wm = "kwin";
        wmCommands = ( QStringList() << "kwin" ); 
        // launch it
        launchWM( QList< QStringList >() << wmCommands );
        return;
    }
}

void KSMServer::autoStart0()
{
    if( state != LaunchingWM )
        return;
    if( !checkStartupSuspend())
        return;
    state = AutoStart0;

    static const QString kdedInterface = QString::fromLatin1("org.kde.kded");
    QDBusConnectionInterface* sessionInterface = QDBusConnection::sessionBus().interface();
    if (!sessionInterface->isServiceRegistered(kdedInterface)) {
        sessionInterface->startService(kdedInterface);
    }
#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif

    klauncherSignals->asyncCall("autoStart", int(0));
}

void KSMServer::autoStart0Done()
{
    if( state != AutoStart0 )
        return;
    disconnect( klauncherSignals, SIGNAL(autoStart0Done()), this, SLOT(autoStart0Done()));
    if( !checkStartupSuspend())
        return;
    kDebug() << "Autostart 0 done";
#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif
    state = KcmInitPhase1;
    kcminitSignals = new QDBusInterface("org.kde.kcminit", "/kcminit", "org.kde.KCMInit", QDBusConnection::sessionBus(), this );
    if( !kcminitSignals->isValid()) {
        kWarning() << "kcminit not running? If we are running with mobile profile or in another platform other than X11 this is normal.";
        delete kcminitSignals;
        kcminitSignals = 0;
        QMetaObject::invokeMethod(this, "kcmPhase1Done", Qt::QueuedConnection);
        return;
    }
    connect( kcminitSignals, SIGNAL(phase1Done()), SLOT(kcmPhase1Done()));
    QTimer::singleShot( 10000, this, SLOT(kcmPhase1Timeout())); // protection

    org::kde::KCMInit kcminit("org.kde.kcminit", "/kcminit" , QDBusConnection::sessionBus());
    kcminit.runPhase1();
}

void KSMServer::kcmPhase1Done()
{
    if( state != KcmInitPhase1 )
        return;
    kDebug() << "Kcminit phase 1 done";
    if (kcminitSignals) {
        disconnect( kcminitSignals, SIGNAL(phase1Done()), this, SLOT(kcmPhase1Done()));
    }
    autoStart1();
}

void KSMServer::kcmPhase1Timeout()
{
    if( state != KcmInitPhase1 )
        return;
    kDebug() << "Kcminit phase 1 timeout";
    kcmPhase1Done();
}

void KSMServer::autoStart1()
{
    if( state != KcmInitPhase1 )
        return;
    state = AutoStart1;
#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif
    klauncherSignals->asyncCall("autoStart", int(1));
}

void KSMServer::autoStart1Done()
{
    if( state != AutoStart1 )
        return;
    disconnect( klauncherSignals, SIGNAL(autoStart1Done()), this, SLOT(autoStart1Done()));
    if( !checkStartupSuspend())
        return;
    kDebug() << "Autostart 1 done";
    setupShortcuts(); // done only here, because it needs kglobalaccel :-/
    lastAppStarted = 0;
    lastIdStarted.clear();
    state = Restoring;
#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif
    if( defaultSession()) {
        autoStart2();
        return;
    }
    tryRestoreNext();
}

void KSMServer::clientRegistered( const char* previousId )
{
    if ( previousId && lastIdStarted == previousId )
        tryRestoreNext();
}

void KSMServer::tryRestoreNext()
{
    if( state != Restoring && state != RestoringSubSession )
        return;
    restoreTimer.stop();
    startupSuspendTimeoutTimer.stop();
    KConfigGroup config(KGlobal::config(), sessionGroup );

    while ( lastAppStarted < appsToStart ) {
        lastAppStarted++;
        QString n = QString::number(lastAppStarted);
        QString clientId = config.readEntry( QString("clientId")+n, QString() );
        bool alreadyStarted = false;
        foreach ( KSMClient *c, clients ) {
            if ( c->clientId() == clientId ) {
                alreadyStarted = true;
                break;
            }
        }
        if ( alreadyStarted )
            continue;

        QStringList restartCommand = config.readEntry( QString("restartCommand")+n, QStringList() );
        if ( restartCommand.isEmpty() ||
             (config.readEntry( QString("restartStyleHint")+n, 0 ) == SmRestartNever)) {
            continue;
        }
        if ( isWM( config.readEntry( QString("program")+n, QString() ) ) )
            continue; // wm already started
        if( config.readEntry( QString( "wasWm" )+n, false ))
            continue; // it was wm before, but not now, don't run it (some have --replace in command :(  )
        startApplication( restartCommand,
                          config.readEntry( QString("clientMachine")+n, QString() ),
                          config.readEntry( QString("userId")+n, QString() ));
        lastIdStarted = clientId;
        if ( !lastIdStarted.isEmpty() ) {
            restoreTimer.setSingleShot( true );
            restoreTimer.start( 2000 );
            return; // we get called again from the clientRegistered handler
        }
    }

    //all done
    appsToStart = 0;
    lastIdStarted.clear();

    if (state == Restoring)
        autoStart2();
    else { //subsession
        state = Idle;
        emit subSessionOpened();
    }
}

void KSMServer::autoStart2()
{
    if( state != Restoring )
        return;
    if( !checkStartupSuspend())
        return;
    state = FinishingStartup;
#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif
    waitAutoStart2 = true;
    waitKcmInit2 = true;
    klauncherSignals->asyncCall("autoStart", int(2));

#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << "klauncher" << t.elapsed();
#endif

    QDBusInterface kded( "org.kde.kded", "/kded", "org.kde.kded" );
    QDBusPendingCall pendingcall = kded.asyncCall( "loadSecondPhase" );
    while (!pendingcall.isFinished()) {
        QApplication::processEvents();
    }

#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << "kded" << t.elapsed();
#endif

    if (kcminitSignals) {
        connect( kcminitSignals, SIGNAL(phase2Done()), SLOT(kcmPhase2Done()));
        QTimer::singleShot( 10000, this, SLOT(kcmPhase2Timeout())); // protection
        org::kde::KCMInit kcminit("org.kde.kcminit", "/kcminit" , QDBusConnection::sessionBus());
        kcminit.runPhase2();
    } else {
        QMetaObject::invokeMethod(this, "kcmPhase2Done", Qt::QueuedConnection);
    }
    KNotification::event("kde/startkde"); // this is the time KDE is up, more or less
}

void KSMServer::autoStart2Done()
{
    if( state != FinishingStartup )
        return;
    disconnect( klauncherSignals, SIGNAL(autoStart2Done()), this, SLOT(autoStart2Done()));
    kDebug() << "Autostart 2 done";
    waitAutoStart2 = false;
    finishStartup();
}

void KSMServer::kcmPhase2Done()
{
    if( state != FinishingStartup )
        return;
    kDebug() << "Kcminit phase 2 done";
    if (kcminitSignals) {
        disconnect( kcminitSignals, SIGNAL(phase2Done()), this, SLOT(kcmPhase2Done()));
        delete kcminitSignals;
        kcminitSignals = 0;
    }
    waitKcmInit2 = false;
    finishStartup();
}

void KSMServer::kcmPhase2Timeout()
{
    if( !waitKcmInit2 )
        return;
    kDebug() << "Kcminit phase 2 timeout";
    kcmPhase2Done();
}

void KSMServer::finishStartup()
{
    if( state != FinishingStartup )
        return;
    if( waitAutoStart2 || waitKcmInit2 )
        return;

#ifdef KSMSERVER_STARTUP_DEBUG1
    kDebug() << t.elapsed();
#endif

    state = Idle;
    setupXIOErrorHandler(); // From now on handle X errors as normal shutdown.
}

bool KSMServer::checkStartupSuspend()
{
    if( startupSuspendCount.isEmpty())
        return true;
    // wait for the phase to finish
    if( !startupSuspendTimeoutTimer.isActive())
    {
        startupSuspendTimeoutTimer.setSingleShot( true );
        startupSuspendTimeoutTimer.start( 10000 );
    }
    return false;
}

void KSMServer::suspendStartup( const QString &app )
{
    if( !startupSuspendCount.contains( app ))
        startupSuspendCount[ app ] = 0;
    ++startupSuspendCount[ app ];
}

void KSMServer::resumeStartup( const QString &app )
{
    if( !startupSuspendCount.contains( app ))
        return;
    if( --startupSuspendCount[ app ] == 0 ) {
        startupSuspendCount.remove( app );
        if( startupSuspendCount.isEmpty() && startupSuspendTimeoutTimer.isActive()) {
            startupSuspendTimeoutTimer.stop();
            resumeStartupInternal();
        }
    }
}

void KSMServer::startupSuspendTimeout()
{
    kDebug() << "Startup suspend timeout:" << state;
    resumeStartupInternal();
}

void KSMServer::resumeStartupInternal()
{
    startupSuspendCount.clear();
    switch( state ) {
        case LaunchingWM:
            autoStart0();
            break;
        case AutoStart0:
            autoStart0Done();
            break;
        case AutoStart1:
            autoStart1Done();
            break;
        case Restoring:
            autoStart2();
            break;
        default:
            kWarning() << "Unknown resume startup state" ;
            break;
    }
}

void KSMServer::restoreSubSession( const QString& name )
{
    sessionGroup = "SubSession: " + name;

    KConfigGroup configSessionGroup( KGlobal::config(), sessionGroup);
    int count =  configSessionGroup.readEntry( "count", 0 );
    appsToStart = count;
    lastAppStarted = 0;
    lastIdStarted.clear();

    state = RestoringSubSession;
    tryRestoreNext();
}
