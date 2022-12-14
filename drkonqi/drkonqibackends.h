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
#ifndef DRKONQIBACKENDS_H
#define DRKONQIBACKENDS_H

#include <QtCore/QObject>

class CrashedApplication;
class DebuggerManager;

class KCrashBackend : public QObject
{
    Q_OBJECT
public:
    KCrashBackend();
    ~KCrashBackend();

    bool init();

    inline CrashedApplication *crashedApplication() const {
        return m_crashedApplication;
    }

    inline DebuggerManager *debuggerManager() const {
        return m_debuggerManager;
    }

private slots:
    void stopAttachedProcess();
    void continueAttachedProcess();

private:
    CrashedApplication *constructCrashedApplication();
    DebuggerManager *constructDebuggerManager();

    static void emergencySaveFunction(int signal);
    static qint64 s_pid; //for use by the emergencySaveFunction

    CrashedApplication *m_crashedApplication;
    DebuggerManager *m_debuggerManager;
};

#endif // DRKONQIBACKENDS_H
