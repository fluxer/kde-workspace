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

#ifndef AA_SCREEN_H
#define AA_SCREEN_H

#include <QObject>
#include <QList>
#include <QPixmap>

class Sprite;
class AASaver;
class QPaintEvent;
class QPainter;

/**
 * This is the main display class of Asciiquarium.  We use a pseudo-terminal-ish
 * type coordinate system, where although this is a full fledged GUI application,
 * Sprites and most external functions deal with logical text position
 * coordinates instead of GUI coordinates.  (x, y) starts in the upper-left of
 * the real screen at (0, 0), and continues on to (width - 1, height - 1).
 *
 * Use addSprite() to add new Sprites to the Screen after you have created them
 * and added their Frames.
 */
class Screen: public QObject
{
    Q_OBJECT

public:
    /**
     * Represents a logical character on the Screen.
     */
    struct Pixel {
        char letter; ///< Character to display in the cell.
        QRgb color;  ///< Color to use for the cell.

        /// Default constructor.
        Pixel(): letter(' '), color(0)
        {
        }
    };

private:
    AASaver* m_widget; ///< Widget that we should paint on.
    int      m_width;  ///< Number of logical columns in the screen.
    int      m_height; ///< Number of logical rows on the screen.
    int      m_offX;   ///< Number of pixels on left side needed to center image.
    int      m_offY;   ///< Number of pixels on top side needed to center image.

    /** Pixmap cache of the image used to speed up rendering.  All paints happen
     *  to the pixmap, which is then bitBlt()'ed to m_widget when the time comes
     *  to paint.
     */
    QPixmap  m_backBuffer;
    QPainter *m_curPainter;

    int      m_cellW; ///< The GUI width of a character cell.
    int      m_cellH; ///< The GUI height of a character cell.

    QList<Sprite*> m_sprites; ///< List of Sprites on screen.
    QList<Sprite*> m_addedSprites; ///< List of Sprites to be added next frame.

private slots:
    /**
     * Handles updating the screen buffer to draw the next frame.
     */
    void doAnimate();

public:
    /**
     * Constructor.
     *
     * @param widget The widget to draw on.
     */
    Screen(AASaver* widget);
    ~Screen();

    /// Returns the logical width of the screen.
    int width() const
    {
        return m_width;
    }

    /// Returns the logical height of the screen.
    int height() const
    {
        return m_height;
    }

    /// Returns the GUI width of a character cell.
    int cellWidth() const
    {
        return m_cellW;
    }

    /// Returns the GUI height of a character cell.
    int cellHeight() const
    {
        return m_cellH;
    }

    /**
     * Adds a sprite to the internal sprite list.
     *
     * @param sprite The Sprite to add.  It will show up in the next frame.
     */
    void addSprite(Sprite* sprite)
    {
        m_addedSprites.append(sprite);
    }

    /// Returns the number of milliseconds separating each animation tick.
    int msPerTick() const;

    /**
     * Updates the backbuffer, and asks the portion of the widget to be
     * repainted.
     *
     * @param x The logical x coordinate of the left edge of the update area.
     * @param y The logical y coordinate of the top edge of the update area.
     * @param updatePixmap The pixmap to draw into the buffer, which should be
     *                     masked to only draw non-transparent regions.
     */
    void updateSpan(int x, int y, const QPixmap &updatePixmap);

    /**
     * Clear the given portion of the backbuffer, asks for a repaint.
     *
     * @param x The logical x coordinate of the left edge of the update region.
     * @param y The logical y coordinate of the top edge of the update region.
     * @param clearPixmap the pixmap to use to clear the span, which should be
     *                    the background color of the Screen, and masked to
     *                    only draw the area that needs cleared.
     */
    void clearSpan(int x, int y, const QPixmap &clearPixmap);

    /**
     * Actually paints the scene on the widget.
     *
     * @param pe The QPaintEvent given to the updating widget.
     */
    void paint(QPaintEvent *pe);
};

#endif

// vim: set et ts=8 sw=4:
