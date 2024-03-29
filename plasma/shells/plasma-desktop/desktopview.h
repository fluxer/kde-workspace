/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef DESKTOPVIEW_H
#define DESKTOPVIEW_H

#include <Plasma/Plasma>
#include <Plasma/View>

#include "desktoptracker.h"

namespace Plasma
{
    class Containment;
} // namespace Plasma

class DesktopView : public Plasma::View
{
    Q_OBJECT

public:
    DesktopView(Plasma::Containment *containment, int id, QWidget *parent);
    ~DesktopView();

    /**
     * Sets things up for per-desktop views (or not)
     */
    void checkDesktopAffiliation();

public slots:
    void screenResized(const DesktopTracker::Screen &screen);
    void screenMoved(const DesktopTracker::Screen &screen);
    void adjustSize();
    void toolBoxOpened(bool);
    void showDesktopUntoggled(WId id);
    void showWidgetExplorer();

    void screenOwnerChanged(int wasScreen, int isScreen, Plasma::Containment* containment);

    /**
     * Sets the containment for this view, which will also cause the view
     * to track the geometry of the containment.
     *
     * @arg containment the containment to center the view on
     */
    void setContainment(Plasma::Containment *containment);

private:
    bool m_init;

    //FIXME: duplicated from containment_p.h 
    //(but with a bigger margin to make room even for very big panels)
    static const int TOOLBOX_MARGIN = 400;
};

#endif // multiple inclusion guard
