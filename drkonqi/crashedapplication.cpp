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

#if defined(HAVE_STRSIGNAL)
# include <clocale>
# include <cstring>
# include <cstdlib>
#else
# include <signal.h>
#endif

#include <KToolInvocation>

CrashedApplication::CrashedApplication(QObject *parent)
    : QObject(parent),
    m_pid(0),
    m_signalNumber(0),
    m_restarted(false),
    m_thread(0)
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

QString CrashedApplication::executableBaseName() const
{
    return m_executable.baseName();
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
#if defined(HAVE_STRSIGNAL)
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
    case SIGILL: return QString::fromLatin1("SIGILL");
    case SIGABRT: return QString::fromLatin1("SIGABRT");
    case SIGFPE: return QString::fromLatin1("SIGFPE");
    case SIGSEGV: return QString::fromLatin1("SIGSEGV");
    case SIGBUS: return QString::fromLatin1("SIGBUS");
    default: return QString::fromLatin1("Unknown");
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

    //start the application via klauncher, as it needs to have a pristine environment and
    //QProcess::startDetached() can't start a new process with custom environment variables.
    int ret = KToolInvocation::kdeinitExec(m_executable.absoluteFilePath());

    const bool success = (ret == 0);

    m_restarted = success;
    emit restarted(success);
}

QString getSuggestedKCrashFilename(const CrashedApplication* app)
{
    QString filename = app->executableBaseName() + '-' +
                       app->datetime().toString("yyyyMMdd-hhmmss") +
                       ".kcrash.txt";

    if (filename.contains('/')) {
        filename = filename.mid(filename.lastIndexOf('/') + 1);
    }

    return filename;
}

#include "moc_crashedapplication.cpp"
