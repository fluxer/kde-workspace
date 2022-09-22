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
#include "debuggerlaunchers.h"

#include <QtDBus/QDBusConnection>

#include <KShell>
#include <KDebug>
#include <KProcess>

#include "detachedprocessmonitor.h"
#include "drkonqi.h"
#include "crashedapplication.h"

DefaultDebuggerLauncher::DefaultDebuggerLauncher(const Debugger & debugger, DebuggerManager *parent)
    : AbstractDebuggerLauncher(parent), m_debugger(debugger)
{
    m_monitor = new DetachedProcessMonitor(this);
    connect(m_monitor, SIGNAL(processFinished()), SLOT(onProcessFinished()));
}

QString DefaultDebuggerLauncher::name() const
{
    return m_debugger.name();
}

void DefaultDebuggerLauncher::start()
{
    if ( qobject_cast<DebuggerManager*>(parent())->debuggerIsRunning() ) {
        kWarning() << "Another debugger is already running";
        return;
    }

    QString str = m_debugger.command();
    Debugger::expandString(str, Debugger::ExpansionUsageShell);

    emit starting();
    QStringList procargs = KShell::splitArgs(str);
    QString procprog = procargs.takeAt(0);
    qint64 procpid = KProcess::startDetached(procprog, procargs);
    if ( procpid > 0 ) {
        m_monitor->startMonitoring(procpid);
    } else {
        kError() << "Could not start debugger:" << name();
        emit finished();
    }
}

void DefaultDebuggerLauncher::onProcessFinished()
{
    emit finished();
}

#if 0
TerminalDebuggerLauncher::TerminalDebuggerLauncher(const Debugger & debugger, DebuggerManager *parent)
    : DefaultDebuggerLauncher(debugger, parent)
{
}

void TerminalDebuggerLauncher::start()
{
    DefaultDebuggerLauncher::start(); //FIXME
}
#endif

#include "moc_debuggerlaunchers.cpp"
