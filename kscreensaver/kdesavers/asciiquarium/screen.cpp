/*
 * Asciiquarium - Native KDE Screensaver based on the Asciiquarium program
 *    (c) Kirk Baucom <kbaucom@schizoid.com>, which you can find at
 *    http://www.robobunny.com/projects/asciiquarium/
 *
 * Ported to KDE by Maksim Orlovich <maksim@kde.org> and
 * Michael Pyne <michael.pyne@kdemail.net>.
 *
 * Copyright (c) 2003 Kirk Baucom     <kbaucom@schizoid.com>
 * Copyright (c) 2005 Maksim Orlovich <maksim@kde.org>
 * Copyright (c) 2005, 2008 Michael Pyne <michael.pyne@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "screen.h"

#include <QtCore/QTimer>
#include <QtCore/QList>
#include <QtGui/QColor>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>

#include <kglobalsettings.h>

#include "sprite.h"
#include "aasaver.h"

Screen::Screen(AASaver* widget): m_widget(widget), m_curPainter(0)
{
    QFontMetrics fm(KGlobalSettings::fixedFont());

    // Compute cell geometries.
    m_cellW  = fm.maxWidth();
    m_cellH  = fm.lineSpacing();

    // Computer number of full cells that will fit.
    m_width  = widget->width()  / m_cellW;
    m_height = widget->height() / m_cellH;

    // Calculate offset needed to evenly distribute excess screen space.
    m_offX = (widget->width() - m_width * m_cellW) / 2;
    m_offY = (widget->height() - m_height * m_cellH) / 2;

    // Setup animation timer.
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), m_widget, SLOT(update()));

    timer->start(msPerTick());
}

int Screen::msPerTick() const
{
    return 50;
}

Screen::~Screen()
{
}

void Screen::updateSpan(int x, int y, const QPixmap &updatePixmap)
{
    if (y < 0 || y >= m_height)
        return;

    QPoint upperLeft(m_offX + x * m_cellW, m_offY + y * m_cellH);
    m_curPainter->drawPixmap(upperLeft, updatePixmap);
}

void Screen::clearSpan(int x, int y, const QPixmap &clearPixmap)
{
    if (y < 0 || y >= m_height)
        return;

    QPoint upperLeft(m_offX + x * m_cellW, m_offY + y * m_cellH);
    m_curPainter->fillRect(QRect(upperLeft, clearPixmap.size()), Qt::black);
}

//Actually paints the region on the widget.
void Screen::paint(QPaintEvent *)
{
    if(m_backBuffer.isNull()) {
        m_backBuffer = QPixmap(m_widget->size());
        m_backBuffer.fill(Qt::black);
    }

    { // Artificial scoping for the QPainter
        QPainter p(&m_backBuffer);

        // XXX: This is a hack to allow updateSpan and clearSpan access to the
        // current QPainter.  Could use some re-architecting.  This sequence
        // draws onto the back buffer.
        m_curPainter = &p;
        doAnimate();
        m_curPainter = 0;
    }

    // Draw onto the main widget now.
    QPainter p(m_widget);
    p.setCompositionMode(QPainter::CompositionMode_Source); // bitBlt ftw
    p.drawPixmap(0, 0, m_backBuffer);
}

/**
 * Utility type used to faciliate sorting of the Sprite list in order to
 * implement the Painter's Algorithm when painting the back buffer.
 */
struct ZKey
{
    /**
     * Logical depth of sprite.  Now 0 is farthest away from the eyes, unlike
     * with Sprite::depth().
     */
    int      z;

    Sprite*  addr;

    ZKey(): z(0), addr(0)
    {}

    ZKey(Sprite* spr): z(1000 - spr->depth()), addr(spr)
    {}

    bool operator<(const ZKey& other) const
    {
        if (z < other.z) return true;
        if (z > other.z) return false;

        return addr < other.addr;
    }
};

void Screen::doAnimate()
{
    //First, rebuild a new list of sprites, and build a dirty region
    QRegion dirtyRegion;

    QList<Sprite*> sprites;
    QList<Sprite*> colliders;

    // Look for sprites that can suffer a collision.
    foreach(Sprite *sprite, m_sprites) {
        if(sprite->canCollide())
            colliders.append(sprite);
    }

    // Find collisions.
    // FIXME: Use transparent regions for accuracy.
    foreach(Sprite *collider, colliders) {
        foreach(Sprite *sprite, m_sprites) {
            // Can't collide with yourself...
            if(sprite == collider)
                continue;

            if(collider->geom().intersects(sprite->geom()))
                collider->collision(sprite);
        }
    }

    //Retain all live existing sprites
    foreach(Sprite *sprite, m_sprites) {
        QRect oldRect = sprite->geom();

        if (!sprite->isKilled()) {
            bool dirty = sprite->tickUpdate();

            if (dirty)
                dirtyRegion |= oldRect | sprite->geom();

            if (!sprite->isKilled())
                sprites.append(sprite);
        }

        if (sprite->isKilled()) //note:may be made true by updateTick!
        {
            dirtyRegion |= oldRect;
            delete sprite;
        }
    }

    //Add new sprites.
    foreach(Sprite *sprite, m_addedSprites) {
        dirtyRegion |= sprite->geom();
        sprites.append(sprite);
    }

    m_addedSprites.clear();
    m_sprites = sprites;

    //Compute the list of sprites affected. Note that this is
    //done iteratively until fixed point.
    QList<Sprite*> paintSprites;
    QList<Sprite*> remSprites;

    bool changed;
    do
    {
        changed = false;
        remSprites.clear();

        foreach(Sprite *sprite, sprites) {
            if (dirtyRegion.intersect(sprite->geom()).isEmpty())
                remSprites.append(sprite); //not to be painted thus far
            else
            {
                //This sprite is to be painted
                paintSprites.append(sprite);

                //make sure we repaint everything overlapping it
                dirtyRegion |= sprite->geom();
                changed = true;
            }
        }

        sprites = remSprites;
    }
    while (changed);

    //Z-sort the items.
    QMap<ZKey, Sprite* > sorted;
    foreach(Sprite *sprite, paintSprites)
        sorted[ZKey(sprite)] = sprite;

    //Paint, in Z-order
    foreach(Sprite *sprite, sorted)
        sprite->paint();
}

#include "screen.moc"

// vim: set et ts=8 sw=4:
