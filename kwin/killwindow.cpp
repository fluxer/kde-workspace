/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 1999, 2000 Matthias Ettrich <ettrich@kde.org>
Copyright (C) 2003 Lubos Lunak <l.lunak@kde.org>
Copyright (C) 2012 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "killwindow.h"

#include <kstandarddirs.h>
#include <kdebug.h>

namespace KWin
{

KillWindow::KillWindow(QObject *parent)
    : QObject(parent)
{
    m_xkill = KStandardDirs::findExe("xkill");
    m_proc = new QProcess(this);
    connect(
        m_proc, SIGNAL(finished(int)),
        this, SLOT(slotProcessFinished(int))
    );
}

KillWindow::~KillWindow()
{
    m_proc->terminate();
}

void KillWindow::start()
{
    if (m_proc->state() == QProcess::Running) {
        kDebug(1212) << "xkill already started";
        return;
    }
    if (m_xkill.isEmpty()) {
        kWarning(1212) << "xkill not found";
        return;
    }
    m_proc->start(m_xkill);
}

void KillWindow::slotProcessFinished(const int exitcode)
{
    if (exitcode != 0) {
        kWarning(1212) << "xkill exited abnormally" << exitcode;
    }
}

} // namespace
