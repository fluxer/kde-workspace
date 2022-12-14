/*
    Copyright (C) 2009  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "drkonqibackends.h"

#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <signal.h>

#include <QtCore/QTimer>
#include <QtCore/QDir>

#include <KCmdLineArgs>
#include <KStandardDirs>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>
#include <KStartupInfo>
#include <KCrash>
#include <kde_file.h>

#include "crashedapplication.h"
#include "debugger.h"
#include "debuggermanager.h"
#include "backtracegenerator.h"

KCrashBackend::KCrashBackend()
    : QObject()
{
}

KCrashBackend::~KCrashBackend()
{
    continueAttachedProcess();
}

bool KCrashBackend::init()
{
    m_crashedApplication = constructCrashedApplication();
    m_debuggerManager = constructDebuggerManager();

    QString startupId(KCmdLineArgs::parsedArgs()->getOption("startupid"));
    if (!startupId.isEmpty()) { // stop startup notification
        KStartupInfoId id;
        id.initId(startupId.toLocal8Bit());
        KStartupInfo::sendFinish(id);
    }

    //check whether the attached process exists and whether we have permissions to inspect it
    if (crashedApplication()->pid() <= 0) {
        kError() << "Invalid pid specified";
        return false;
    }

    if (::kill(crashedApplication()->pid(), 0) < 0) {
        switch (errno) {
        case EPERM:
            kError() << "DrKonqi doesn't have permissions to inspect the specified process";
            break;
        case ESRCH:
            kError() << "The specified process does not exist.";
            break;
        default:
            break;
        }
        return false;
    }

    // stop the process to avoid high cpu usage by other threads (bug 175362), also to get a
    // backtrace the process must not exit
    stopAttachedProcess();

    // --keeprunning means: generate backtrace instantly and let the process continue execution
    if (KCmdLineArgs::parsedArgs()->isSet("keeprunning")) {
        connect(debuggerManager(), SIGNAL(debuggerFinished()), SLOT(continueAttachedProcess()));
        debuggerManager()->backtraceGenerator()->start();
    }

    //Handle drkonqi crashes
    s_pid = crashedApplication()->pid(); //copy pid for use by the crash handler, so that it is safer
    KCrash::setCrashHandler(emergencySaveFunction);

    return true;
}

CrashedApplication *KCrashBackend::constructCrashedApplication()
{
    CrashedApplication *a = new CrashedApplication(this);
    a->m_datetime = QDateTime::currentDateTime();
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    a->m_name = args->getOption("programname");
    a->m_version = args->getOption("appversion").toUtf8();
    a->m_reportAddress = args->getOption("bugaddress").toUtf8();
    a->m_pid = args->getOption("pid").toInt();
    a->m_signalNumber = args->getOption("signal").toInt();
    a->m_restarted = args->isSet("restarted");
    a->m_thread = args->getOption("thread").toInt();

    //try to determine the executable that crashed
    if (!args->getOption("apppath").isEmpty()) {
        a->m_executable = QFileInfo(args->getOption("apppath"));
    } else if (!args->getOption("appname").isEmpty() ) {
        a->m_executable = QFileInfo(KStandardDirs::findExe(args->getOption("appname")));
    } else {
        kError() << "Neither apppath nor appname are set";
    }

    kDebug() << "Executable is:" << a->m_executable.absoluteFilePath();
    kDebug() << "Executable exists:" << a->m_executable.exists();

    return a;
}

DebuggerManager *KCrashBackend::constructDebuggerManager()
{
    QList<Debugger> internalDebuggers = Debugger::availableInternalDebuggers("KCrash");
    KConfigGroup config(KGlobal::config(), "DrKonqi");
    QString defaultDebuggerName = config.readEntry("Debugger", QString("gdb"));

    Debugger firstKnownGoodDebugger, preferredDebugger;
    foreach (const Debugger & debugger, internalDebuggers) {
        if (!firstKnownGoodDebugger.isValid() && debugger.isInstalled()) {
            firstKnownGoodDebugger = debugger;
        }
        if (debugger.codeName() == defaultDebuggerName) {
            preferredDebugger = debugger;
        }
        if (firstKnownGoodDebugger.isValid() && preferredDebugger.isValid()) {
            break;
        }
    }

    if (!preferredDebugger.isInstalled()) {
        if (firstKnownGoodDebugger.isValid()) {
            preferredDebugger = firstKnownGoodDebugger;
        } else {
            kError() << "Unable to find an internal debugger that can work with the KCrash backend";
        }
    }

    return new DebuggerManager(preferredDebugger, Debugger::availableExternalDebuggers("KCrash"), this);
}

void KCrashBackend::stopAttachedProcess()
{
    kDebug() << "Sending SIGSTOP to process";
    ::kill(crashedApplication()->pid(), SIGSTOP);
}

void KCrashBackend::continueAttachedProcess()
{
    kDebug() << "Sending SIGCONT to process";
    ::kill(crashedApplication()->pid(), SIGCONT);
}

//static
qint64 KCrashBackend::s_pid = 0;

//static
void KCrashBackend::emergencySaveFunction(int signal)
{
    KDE_signal(signal, SIG_DFL);

    // In case drkonqi itself crashes, we need to get rid of the process being debugged,
    // so we kill it, no matter what its state was.
    ::kill(s_pid, SIGKILL);

    ::exit(signal);
}

#include "moc_drkonqibackends.cpp"
