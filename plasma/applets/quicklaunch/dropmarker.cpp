/***************************************************************************
 *   Copyright (C) 2010 - 2011 by Ingomar Wesp <ingomar@wesp.name>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/
#include "dropmarker.h"

// Plasma
#include <Plasma/Theme>

// Own
#include "launcherdata.h"
#include "dropmarker.h"

namespace Quicklaunch {

DropMarker::DropMarker(QGraphicsWidget *parent)
    : Launcher(LauncherData(), parent)
{
    hide();
}

void DropMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // This mirrors the behavior of the panel spacer committed by mart
    // workspace/plasma/desktop/containments/panel/panel.cpp R875513)
    QColor brushColor(Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
    brushColor.setAlphaF(0.3);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(brushColor));

    painter->drawRoundedRect(contentsRect(), 4, 4);
    Launcher::paint(painter, option, widget);
}

}
