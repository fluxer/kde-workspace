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

#include <config-drkonqi.h>

#include "crashedapplication.h"

#if defined(HAVE_STRSIGNAL) && defined(Q_OS_UNIX)
# include <clocale>
# include <cstring>
# include <cstdlib>
#else
# if defined(Q_OS_UNIX)
#  include <signal.h>
# else
#  include <windows.h>
# endif
#endif

#include <KToolInvocation>

CrashedApplication::CrashedApplication(QObject *parent)
    : QObject(parent), m_restarted(false)
{
}

CrashedApplication::~CrashedApplication()
{
}

QString CrashedApplication::name() const
{
    return m_name;
}

QFileInfo CrashedApplication::executable() const
{
    return m_executable;
}

QString CrashedApplication::fakeExecutableBaseName() const
{
    if (!m_fakeBaseName.isEmpty()) {
        return m_fakeBaseName;
    } else {
        return m_executable.baseName();
    }
}

QString CrashedApplication::version() const
{
    return m_version;
}

QString CrashedApplication::bugReportAddress() const
{
    return m_reportAddress;
}

int CrashedApplication::pid() const
{
    return m_pid;
}

int CrashedApplication::signalNumber() const
{
    return m_signalNumber;
}

QString CrashedApplication::signalName() const
{
#if defined(HAVE_STRSIGNAL) && defined(Q_OS_UNIX)
    const char * oldLocale = std::setlocale(LC_MESSAGES, NULL);
    char * savedLocale;
    if (oldLocale) {
        savedLocale = strdup(oldLocale);
    } else {
        savedLocale = NULL;
    }
    std::setlocale(LC_MESSAGES, "C");
    const char *name = strsignal(m_signalNumber);
    std::setlocale(LC_MESSAGES, savedLocale);
    std::free(savedLocale);
    return QString::fromLocal8Bit(name != NULL ? name : "Unknown");
#else
    switch (m_signalNumber) {
# if defined(Q_OS_UNIX)
    case SIGILL: return QLatin1String("SIGILL");
    case SIGABRT: return QLatin1String("SIGABRT");
    case SIGFPE: return QLatin1String("SIGFPE");
    case SIGSEGV: return QLatin1String("SIGSEGV");
    case SIGBUS: return QLatin1String("SIGBUS");
# else
    case EXCEPTION_ACCESS_VIOLATION: return QLatin1String("EXCEPTION_ACCESS_VIOLATION");
    case EXCEPTION_DATATYPE_MISALIGNMENT: return QLatin1String("EXCEPTION_DATATYPE_MISALIGNMENT");
    case EXCEPTION_BREAKPOINT: return QLatin1String("EXCEPTION_BREAKPOINT");
    case EXCEPTION_SINGLE_STEP: return QLatin1String("EXCEPTION_SINGLE_STEP");
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return QLatin1String("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
    case EXCEPTION_FLT_DENORMAL_OPERAND: return QLatin1String("EXCEPTION_FLT_DENORMAL_OPERAND");
    case EXCEPTION_FLT_DIVIDE_BY_ZERO: return QLatin1String("EXCEPTION_FLT_DIVIDE_BY_ZERO");
    case EXCEPTION_FLT_INEXACT_RESULT: return QLatin1String("EXCEPTION_FLT_INEXACT_RESULT");
    case EXCEPTION_FLT_INVALID_OPERATION: return QLatin1String("EXCEPTION_FLT_INVALID_OPERATION");
    case EXCEPTION_FLT_OVERFLOW: return QLatin1String("EXCEPTION_FLT_OVERFLOW");
    case EXCEPTION_FLT_STACK_CHECK: return QLatin1String("EXCEPTION_FLT_STACK_CHECK");
    case EXCEPTION_FLT_UNDERFLOW: return QLatin1String("EXCEPTION_FLT_UNDERFLOW");
    case EXCEPTION_INT_DIVIDE_BY_ZERO: return QLatin1String("EXCEPTION_INT_DIVIDE_BY_ZERO");
    case EXCEPTION_INT_OVERFLOW: return QLatin1String("EXCEPTION_INT_OVERFLOW");
    case EXCEPTION_PRIV_INSTRUCTION: return QLatin1String("EXCEPTION_PRIV_INSTRUCTION");
    case EXCEPTION_IN_PAGE_ERROR: return QLatin1String("EXCEPTION_IN_PAGE_ERROR");
    case EXCEPTION_ILLEGAL_INSTRUCTION: return QLatin1String("EXCEPTION_ILLEGAL_INSTRUCTION");
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return QLatin1String("EXCEPTION_NONCONTINUABLE_EXCEPTION");
    case EXCEPTION_STACK_OVERFLOW: return QLatin1String("EXCEPTION_STACK_OVERFLOW");
    case EXCEPTION_INVALID_DISPOSITION: return QLatin1String("EXCEPTION_INVALID_DISPOSITION");
# endif
    default: return QLatin1String("Unknown");
    }
#endif
}

bool CrashedApplication::hasBeenRestarted() const
{
    return m_restarted;
}

int CrashedApplication::thread() const
{
    return m_thread;
}

const QDateTime& CrashedApplication::datetime() const
{
    return m_datetime;
}

void CrashedApplication::restart()
{
    if (m_restarted) {
        return;
    }

    int ret = -1;

    //start the application via kdeinit, as it needs to have a pristine environment and
    //QProcess::startDetached() can't start a new process with custom environment variables.
    if (!m_fakeBaseName.isEmpty()) {
        // if m_fakeBaseName is set, this means m_executable is the path to kdeinit4
        // so we need to use the fakeBaseName to restart the app
        ret = KToolInvocation::kdeinitExec(m_fakeBaseName);
    } else {
        ret = KToolInvocation::kdeinitExec(m_executable.absoluteFilePath());
    }

    const bool success = (ret == 0);

    m_restarted = success;
    emit restarted(success);
}

QString getSuggestedKCrashFilename(const CrashedApplication* app)
{
    QString filename = app->fakeExecutableBaseName() + '-' +
                       app->datetime().toString("yyyyMMdd-hhmmss") +
                       ".kcrash.txt";

    if (filename.contains('/')) {
        filename = filename.mid(filename.lastIndexOf('/') + 1);
    }

    return filename;
}

#include "moc_crashedapplication.cpp"
