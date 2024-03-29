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

#ifndef KWIN_KILLWINDOW_H
#define KWIN_KILLWINDOW_H

#include <QProcess>

namespace KWin
{

class KillWindow : QObject
{
    Q_OBJECT
public:
    KillWindow(QObject *parent);
    ~KillWindow();

    void start();

private Q_SLOTS:
    void slotProcessFinished(const int exitcode);

private:
    QString m_xkill;
    QProcess* m_proc;
};

} // namespace

#endif
