/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2009 Lucas Murray <lmurray@undefinedfire.com>

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

#include "snaphelper.h"

#ifdef KWIN_BUILD_COMPOSITE
#include <kwinxrenderutils.h>
#include <xcb/render.h>
#endif

namespace KWin
{

SnapHelperEffect::SnapHelperEffect()
    : m_active(false)
    , m_window(NULL)
{
    m_timeline.setEasingCurve(QEasingCurve(QEasingCurve::Linear));
    reconfigure(ReconfigureAll);
    connect(effects, SIGNAL(windowClosed(KWin::EffectWindow*)), this, SLOT(slotWindowClosed(KWin::EffectWindow*)));
    connect(effects, SIGNAL(windowStartUserMovedResized(KWin::EffectWindow*)), this, SLOT(slotWindowStartUserMovedResized(KWin::EffectWindow*)));
    connect(effects, SIGNAL(windowFinishUserMovedResized(KWin::EffectWindow*)), this, SLOT(slotWindowFinishUserMovedResized(KWin::EffectWindow*)));
    connect(effects, SIGNAL(windowGeometryShapeChanged(KWin::EffectWindow*,QRect)), this, SLOT(slotWindowResized(KWin::EffectWindow*,QRect)));
}

SnapHelperEffect::~SnapHelperEffect()
{
}

void SnapHelperEffect::reconfigure(ReconfigureFlags)
{
    m_timeline.setDuration(animationTime(250));
}

void SnapHelperEffect::prePaintScreen(ScreenPrePaintData &data, int time)
{
    double oldValue = m_timeline.currentValue();
    if (m_active)
        m_timeline.setCurrentTime(m_timeline.currentTime() + time);
    else
        m_timeline.setCurrentTime(m_timeline.currentTime() - time);
    if (oldValue != m_timeline.currentValue())
        effects->addRepaintFull();
    effects->prePaintScreen(data, time);
}

void SnapHelperEffect::postPaintScreen()
{
    effects->postPaintScreen();
    if (m_timeline.currentValue() != 0.0) {
        // Display the guide
        if ( effects->compositingType() == XRenderCompositing ) {
#ifdef KWIN_BUILD_COMPOSITE
            for (int i = 0; i < effects->numScreens(); ++i) {
                const QRect& rect = effects->clientArea( ScreenArea, i, 0 );
                int midX = rect.x() + rect.width() / 2;
                int midY = rect.y() + rect.height() / 2 ;
                int halfWidth = m_window->width() / 2;
                int halfHeight = m_window->height() / 2;

                xcb_rectangle_t rects[6];
                // Center lines
                rects[0].x = rect.x() + rect.width() / 2 - 2;
                rects[0].y = rect.y();
                rects[0].width = 4;
                rects[0].height = rect.height();
                rects[1].x = rect.x();
                rects[1].y = rect.y() + rect.height() / 2 - 2;
                rects[1].width = rect.width();
                rects[1].height = 4;

                // Window outline
                // The +/- 4 is to prevent line overlap
                rects[2].x = midX - halfWidth + 4;
                rects[2].y = midY - halfHeight;
                rects[2].width = 2*halfWidth - 4;
                rects[2].height = 4;
                rects[3].x = midX + halfWidth - 4;
                rects[3].y = midY - halfHeight + 4;
                rects[3].width = 4;
                rects[3].height = 2*halfHeight - 4;
                rects[4].x = midX - halfWidth;
                rects[4].y = midY + halfHeight - 4;
                rects[4].width = 2*halfWidth - 4;
                rects[4].height = 4;
                rects[5].x = midX - halfWidth;
                rects[5].y = midY - halfHeight;
                rects[5].width = 4;
                rects[5].height = 2*halfHeight - 4;

                xcb_render_fill_rectangles(connection(), XCB_RENDER_PICT_OP_OVER, effects->xrenderBufferPicture(),
                                           preMultiply(QColor(128, 128, 128, m_timeline.currentValue()*128)), 6, rects);
            }
#endif
        }
    } else if (m_window && !m_active) {
        if (m_window->isDeleted())
            m_window->unrefWindow();
        m_window = NULL;
    }
}

void SnapHelperEffect::slotWindowClosed(EffectWindow* w)
{
    if (m_window == w) {
        m_window->refWindow();
        m_active = false;
    }
}

void SnapHelperEffect::slotWindowStartUserMovedResized(EffectWindow *w)
{
    if (w->isMovable()) {
        m_active = true;
        m_window = w;
        effects->addRepaintFull();
    }
}

void SnapHelperEffect::slotWindowFinishUserMovedResized(EffectWindow *w)
{
    Q_UNUSED(w)
    if (m_active) {
        m_active = false;
        effects->addRepaintFull();
    }
}

void SnapHelperEffect::slotWindowResized(KWin::EffectWindow *w, const QRect &oldRect)
{
    if (w == m_window) {
        QRect r(oldRect);
        for (int i = 0; i < effects->numScreens(); ++i) {
            r.moveCenter(effects->clientArea( ScreenArea, i, 0 ).center());
            effects->addRepaint(r);
        }
    }
}

bool SnapHelperEffect::isActive() const
{
    return m_active || m_timeline.currentValue() != 0.0;
}

} // namespace
