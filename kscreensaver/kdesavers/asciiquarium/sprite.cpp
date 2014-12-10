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

#include "sprite.h"

#include <QtCore/QRect>

Sprite::Sprite(Screen* screen, int x, int y, int z, int frameDelay):
    m_screen(screen), m_currentFrame(0), m_x(x), m_y(y), m_z(z),
    m_isKilled(false), m_killAfterLastFrame(false),
    m_ticksSinceFrameChange(0), m_frameDelay(frameDelay)
{
}

void Sprite::addFrame(const Frame& frame)
{
    m_frames.append(frame);
}

void Sprite::erase()
{
    m_frames[m_currentFrame].erase(m_screen, m_x, m_y);
}

void Sprite::paint()
{
    m_frames[m_currentFrame].paint(m_screen, m_x, m_y);
}

bool Sprite::timerTick()
{
    ++m_ticksSinceFrameChange;
    if (m_ticksSinceFrameChange * m_screen->msPerTick() < m_frameDelay)
        return false;

    //Ring! Ring!
    m_ticksSinceFrameChange = 0;
    return true;
}

bool Sprite::tickUpdate()
{
    if (m_frames.size() == 1)
        return false;

    if (!timerTick())
        return false;

    erase();

    ++m_currentFrame;
    if (m_currentFrame == m_frames.size())
    {
        m_currentFrame = 0;

        if(m_killAfterLastFrame)
        {
            erase();
            kill();
        }
    }

    return true;
}

QRect Sprite::geom() const
{
    return QRect(m_x, m_y, m_frames[0].width(), m_frames[0].height());
}


// vim: set et ts=8 sw=4:
