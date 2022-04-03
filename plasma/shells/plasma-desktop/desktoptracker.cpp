/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "desktoptracker.h"

#include <QApplication>
#include <QDesktopWidget>
#include <KDebug>

Q_GLOBAL_STATIC(DesktopTracker, globaldesktoptracker);

DesktopTracker::Screen::Screen()
    : id(-1)
{
}

DesktopTracker::DesktopTracker(QObject *parent)
    : QObject(parent)
{
    for (int i = 0; i < QApplication::desktop()->screenCount(); i++) {
        DesktopTracker::Screen screen;
        screen.id = i;
        screen.geom = QApplication::desktop()->screenGeometry(i);
        screen.position = screen.geom.topLeft();
        m_screens.append(screen);
    }
    qStableSort(m_screens);

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(slotResized(int)));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(slotScreenCountChanged(int)));
}

DesktopTracker* DesktopTracker::self()
{
    return globaldesktoptracker();
}

QRect DesktopTracker::desktopGeometry()
{
    QRect desktopgeom;
    foreach (const DesktopTracker::Screen &screen, self()->screens()) {
        desktopgeom |= screen.geom;
    }
    return desktopgeom;
}

DesktopTracker::Screen DesktopTracker::primaryScreen() const
{
    const int primaryscreen = QApplication::desktop()->primaryScreen();
    foreach (const DesktopTracker::Screen &screen, screens()) {
        if (screen.id == primaryscreen) {
            return screen;
        }
    }
    kDebug() << "No primary screen";
    return DesktopTracker::Screen();
}

QList<DesktopTracker::Screen> DesktopTracker::screens() const
{
    return m_screens;
}

void DesktopTracker::slotResized(int screennumber)
{
    for (int i = 0; i < m_screens.size(); i++) {
        DesktopTracker::Screen screen = m_screens.at(i);
        if (screen.id == screennumber) {
            const QPoint oldposition = screen.position;
            screen.geom = QApplication::desktop()->screenGeometry(screen.id);
            screen.position = screen.geom.topLeft();
            m_screens.removeAt(i);
            m_screens.append(screen);
            qStableSort(m_screens);
            emit screenResized(screen);
            if (oldposition != screen.position) {
                emit screenMoved(screen);
            }
            return;
        }
    }
    kDebug() << "Untracked screen resized" << screennumber;
}

void DesktopTracker::slotScreenCountChanged(int count)
{
    // this is bogus but it is how kephal did it
    for (int i = m_screens.size(); i < count; i++) {
        DesktopTracker::Screen screen;
        screen.id = i;
        screen.geom = QApplication::desktop()->screenGeometry(i);
        screen.position = screen.geom.topLeft();
        m_screens.append(screen);
        qStableSort(m_screens);
        emit screenAdded(screen);
    }

    while (m_screens.size() > count) {
        DesktopTracker::Screen screen = m_screens.takeLast();
        emit screenRemoved(screen);
    }
}

#include "moc_desktoptracker.cpp"
