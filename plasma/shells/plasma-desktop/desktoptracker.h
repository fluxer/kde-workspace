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

#ifndef DESKTOPTRACKER_H
#define DESKTOPTRACKER_H

#include <QObject>
#include <QRect>

class DesktopTracker : public QObject
{
    Q_OBJECT
public:
    class Screen 
    {
    public:
        Screen();

        int id;
        QRect geom;

        bool operator==(const Screen &other) const
        { return id == other.id && geom == other.geom; }

        bool operator<(const Screen &other) const
        { return id < other.id; }
    };

    DesktopTracker(QObject *parent = nullptr);

    static DesktopTracker* self();

    static QRect desktopGeometry();

    DesktopTracker::Screen primaryScreen() const;
    QList<DesktopTracker::Screen> screens() const;
    
Q_SIGNALS:
    void screenResized(DesktopTracker::Screen screen);
    void screenAdded(DesktopTracker::Screen screen);
    void screenMoved(DesktopTracker::Screen screen);
    void screenRemoved(DesktopTracker::Screen screen);

private Q_SLOTS:
    void slotResized(int screennumber);
    void slotScreenCountChanged(int screencount);

private:
    QList<DesktopTracker::Screen> m_screens;
};

#endif // DESKTOPTRACKER_H
