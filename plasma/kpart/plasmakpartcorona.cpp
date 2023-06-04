/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2010 Ryan Rix <ry@n.rix.si>
 *   Copyright 2010 Siddharth Sharma <siddharth.kde@gmail.com>
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
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

#include "plasmakpartcorona.h"

#include "plasmakpart.h"

#include <KDebug>

#include <Plasma/Containment>

#include <qaction.h>
#include <qfile.h>
#include "scripting/scriptengine.h"

PlasmaKPartCorona::PlasmaKPartCorona(QObject *parent)
    : Plasma::Corona(parent)
{
    enableAction("Lock Widgets", false);
    enableAction("Shortcut Settings", false);
    setDefaultContainmentPlugin("newspaper");
}

void PlasmaKPartCorona::loadDefaultLayout()
{
    // used to force a save into the config file
    Plasma::Containment *c = addContainment(QString());

    if (!c) {
        // do some error reporting?
        return;
    }

    requestConfigSync();
}

#include "moc_plasmakpartcorona.cpp"
