/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2007 Lubos Lunak <l.lunak@kde.org>
Copyright (C) 2010 Martin Gräßlin <mgraesslin@kde.org>

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

#include "showpaint.h"

#include <config-kwin.h>

#ifdef KWIN_BUILD_COMPOSITE
#include <xcb/render.h>
#endif

#include <math.h>

#include <qcolor.h>

namespace KWin
{

static QColor colors[] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
                           Qt::yellow, Qt::gray
                         };

ShowPaintEffect::ShowPaintEffect()
    : color_index(0)
{
}

ShowPaintEffect::~ShowPaintEffect()
{
}

void ShowPaintEffect::paintScreen(int mask, QRegion region, ScreenPaintData& data)
{
    painted = QRegion();
    effects->paintScreen(mask, region, data);
#ifdef KWIN_BUILD_COMPOSITE
    if (effects->compositingType() == XRenderCompositing) {
        xcb_render_color_t col;
        float alpha = 0.2;
        const QColor& color = colors[ color_index ];
        col.alpha = int(alpha * 0xffff);
        col.red = int(alpha * 0xffff * color.red() / 255);
        col.green = int(alpha * 0xffff * color.green() / 255);
        col.blue = int(alpha * 0xffff * color.blue() / 255);
        QVector<xcb_rectangle_t> rects;
        foreach (const QRect & r, painted.rects()) {
            xcb_rectangle_t rect = {int16_t(r.x()), int16_t(r.y()), uint16_t(r.width()), uint16_t(r.height())};
            rects << rect;
        }
        xcb_render_fill_rectangles(connection(), XCB_RENDER_PICT_OP_OVER, effects->xrenderBufferPicture(), col, rects.count(), rects.constData());
    }
#endif
    if (++color_index == sizeof(colors) / sizeof(colors[ 0 ]))
        color_index = 0;
}

void ShowPaintEffect::paintWindow(EffectWindow* w, int mask, QRegion region, WindowPaintData& data)
{
    painted |= region;
    effects->paintWindow(w, mask, region, data);
}

} // namespace
