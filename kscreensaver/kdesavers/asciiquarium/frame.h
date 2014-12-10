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

#ifndef AA_FRAME_H
#define AA_FRAME_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QPixmap>

#include "screen.h"

/**
 * Represents a single frame of a sprite's animation.
 *
 * @see Sprite
 */
class Frame
{
    /**
     * Two-dimensional array of Pixels, which represent the appearance of this
     * frame.  This is used to create m_pixmap and m_erasePixmap when they are
     * needed.
     *
     * @see Pixel
     */
    QVector<QVector<Screen::Pixel> > m_data;

    /// Masked pixmap of the animation frame.  Created by convertDataToPixmap().
    QPixmap m_pixmap;

    /// Masked pixmap used to clear frame.  Created by convertDataToPixmap().
    QPixmap m_erasePixmap;

    /// Height of this frame of animation in logical coordinates.
    int m_height;

    /// Width of this frame of animation in logical coordinates.
    int m_width;

    /// Character to be used as a special 'transparent' character.  Normally is
    /// the '?' character.
    QChar m_transparentChar;

public:

    /**
     * Constructs an empty animation Frame.  Do not insert this into a Sprite.
     */
    Frame() : m_height(0), m_width(0)
    {
    }

    /**
     * Constructs an animation frame.
     *
     * @param text Newline-separated text used to construct the Pixel arrays.
     *             The lines do not have to be equal length, any extra needed
     *             characters will automatically be filled with transparency.
     *             Any whitespace at the beginning of a line is converted to
     *             transparency as well.
     *
     * @param mask Newline-separated text used to mask \p text's colors.  This
     *             can be empty or null in which case no masking is performed.
     *             However, if present, there should be the same number of
     *             lines in \p mask as in \p text, although individual lines
     *             can be shorter or empty as convienient.  You can use letters
     *             to stand for colors, e.g. 'r' will make the letter in \p
     *             text at the same position dark red.
     *
     * @param defaultColor The default color to apply to characters.  This
     *                     color is used for all characters in \p text that are
     *                     not altered by \p mask.
     *
     * @param transparent  The character to use to represent transparent areas
     *                     in \p text.  This can be useful when the
     *                     auto-transparency feature can't detect transparent
     *                     areas.
     */
    Frame(const QString& text, const QString& mask, const QRgb& defaultColor, QChar transparent = QLatin1Char('?'));

    /**
     * Paints this Frame into the given screen.
     *
     * @param scr The Screen to draw into.
     * @param x The logical x coordinate of the left edge of the update region.
     * @param y The logical y coordinate of the top edge of the update region.
     */
    void paint(Screen* scr, int x, int y);

    /**
     * Erases this Frame from the given screen.
     *
     * @param scr The Screen to draw into.
     * @param x The logical x coordinate of the left edge of the update region.
     * @param y The logical y coordinate of the top edge of the update region.
     */
    void erase(Screen* scr, int x, int y);

    /// Returns the logical width of this frame.
    int width() const
    {
        return m_width;
    }

    /// Returns the logical height of this frame.
    int height() const
    {
        return m_height;
    }

protected:

    /**
     * This function converts the Pixel data in m_data to setup m_pixmap
     * and m_erasePixmap, which are not setup until this function is called.
     *
     * m_data is not valid after this call is performed to save memory.
     *
     * @param screen The Screen we will be drawing into later.
     */
    void convertDataToPixmap(const Screen *screen);
};

#endif

// vim: set et ts=8 sw=4:
