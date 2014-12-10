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

#ifndef AA_SPRITE_H
#define AA_SPRITE_H

#include <QVector>

#include "frame.h"

/**
 * This class represents a on-screen character of some sort.  These make up
 * the building blocks of the animation.
 *
 * You can use multiple frames of animation, but movement is not supported in
 * this class, try MovingSprite.  If you use multiple frames, use
 * setFrameDelay to control the interval between frames, and use
 * setDieAfterLastFrame to set whether the animation should loop or cause
 * the Sprite to go away.
 *
 * Use kill() to get rid of a Sprite, do not delete it by yourself, as Screen
 * will do that as needed.
 */
class Sprite
{
protected:
    Screen* m_screen;   ///< The Screen that we belong to.
    int m_currentFrame; ///< The current frame of animation.
    int m_x;  ///< Our current logical x position.
    int m_y;  ///< Our current logical y position.
    int m_z;  ///< Our current depth.

    QVector<Frame> m_frames;      ///< Array of animation frames.
    bool m_isKilled;              ///< True if we've been killed.
    bool m_killAfterLastFrame;    ///< True if we should auto-kill after the last frame.
    int  m_ticksSinceFrameChange; ///< Number of timer ticks since we last changed frame.
    int  m_frameDelay;            ///< Number of milliseconds to show a frame for.

    /**
     * Increments the animation timer.
     *
     * @return true if time has elapsed past m_frameDelay since the last frame
     * change.
     */
    bool timerTick();

public:
    /**
     * Construct a sprite without automatically adding it to \p screen.
     *
     * @param screen The Screen that the sprite belongs to.
     * @param x The x column position for the left edge of this sprite.
     * @param y The y row position for the upper line of this sprite.
     * @param z The depth of the sprite (0 is closest to screen).
     * @param frameDelay Amount of milliseconds to elapse between animation
     *                   frames.
     */
    Sprite(Screen* screen, int x, int y, int z, int frameDelay = 100);

    /**
     * Destuctor.  Does nothing at this point, present to ensure a continuous
     * line of virtual destructors.
     */
    virtual ~Sprite()
    {
    }

    /**
     * @return true if this sprite can be involved in a collision with another
     *         Sprite.  The other sprite doesn't necessarily have to have this
     *         also set to true.
     */
    virtual bool canCollide() const { return false; }

    /**
     * Called when a collision occurs with *any* Sprite on-screen if canCollide()
     * returns true.
     *
     * @param sprite The Sprite that a collision happened with.  It is safe to
     *               kill() the Sprite, move it, etc.
     */
    virtual void collision (Sprite *sprite)
    {
        Q_UNUSED(sprite); // Base class does nothing.
    }

    /**
     * Appends a frame of animation to the end of the current list.
     *
     * @param frame Frame of animation to add.  It should be the same size as
     *              the other frames already in the list.
     */
    void addFrame(const Frame& frame);

    /**
     * Sets the amount of time to show a frame for.
     *
     * @param delay The frame delay, in milliseconds of time.
     */
    void setFrameDelay(int delay)
    {
        m_frameDelay = delay;
    }

    /**
     * Sets whether this Sprite should automatically call kill() after the
     * last frame of animation has run.
     *
     * @param dieAfterLast If true, this Sprite will automatically call kill()
     *                     after its last frame has elapsed.
     */
    void setDieAfterLastFrame(bool dieAfterLast)
    {
        m_killAfterLastFrame = dieAfterLast;
    }

    /**
     * @return The Screen this Sprite belongs to.
     */
    Screen *screen() const
    {
        return m_screen;
    }

    /**
     * @return true if this Sprite is dead.  If true, it will probably soon be
     * deleted by its Screen.
     */
    bool isKilled() const
    {
        return m_isKilled;
    }

    /**
     * @return The depth of the Sprite.  0 is closest to the screen.
     */
    int depth() const
    {
        return m_z;
    }

    /**
     * @return The rectangular geometry of this object in the Pixel coordinate
     * system.
     */
    QRect geom() const;

    /**
     * Erases this Sprite from its Screen, using the current animation frame to
     * form the clear mask.  This should be called *before* any change which
     * will change the on-screen display of the object, such as motion or
     * animation changes.
     */
    void erase();

    /**
     * Draws this Sprite onto the Screen.
     */
    void paint();

    /**
     * Kills this Sprite.  The parent Screen will delete this Sprite on the next
     * animation cycle.
     */
    virtual void kill()
    {
        m_isKilled = true;
    }

    //main animation hook. Should return true + erase if something changed
    /**
     * Called when the current frame expires.  This function needs to perform
     * any actions necessary to make sure that it is ready to be painted,
     * including calling erase().  You do not need to call paint() from this
     * function.
     *
     * @return true if the on-screen representation of this Sprite changed,
     *         false, otherwise.
     */
    virtual bool tickUpdate();
};

#endif

// vim: set et ts=8 sw=4:
