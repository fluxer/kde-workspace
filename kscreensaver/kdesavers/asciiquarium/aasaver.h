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

#ifndef AA_AASAVER_H
#define AA_AASAVER_H

#include <kscreensaver.h>
#include <KRandomSequence>

class Screen;
class Sprite;

/**
 * \mainpage Asciiquarium.
 *
 * \section intro Introduction
 *
 * Asciiquarium is a KDE screensaver to draw an ASCII art aquarium.  This is the
 * documentation of the API used in the program to generate the effect.  It should
 * be fairly simple, but basically:
 *
 * class AASaver is the main class, which handles outside events.  All of the
 * processing happens in the Screen class however, which manages a list of
 * Sprites, updating them and drawing them as needed.  When AASaver receives a
 * paintEvent(), it forwards it on to Screen to handle it.
 *
 * Each Sprite is composed of 1 or more Frames.  When a Screen wants a Sprite
 * to draw itself, the Sprite forwards the request to its currently shown Frame.
 *
 * The Frame is rectangular, and created from textual ASCII art, with a ASCII
 * art shape and color mask.  The mask is optional.  See aasaver.cpp for
 * examples for creating a Frame.
 *
 * The Frame supports transparency and colors, and will convert the textual data
 * into a QPixmap representation on demand in order to reduce CPU load (at the
 * expense of a slight memory usage increase for each sprite).
 *
 * Screen handles the timing for the project, and at each timeout will call
 * Sprite::tickUpdate() from Screen::doAnimate().
 *
 * This whole program was inspired/copied from Kirk Baucom's asciiquarium
 * program, from http://www.robobunny.com/projects/asciiquarium/
 */

/**
 * The main class for the Asciiquarium screensaver.
 */
class AASaver: public KScreenSaver
{
    /// Handles the animation and drawing.
    Screen* screen;

    /// Randomizes the sequence of fish, seaweeds, colors, etc.
    KRandomSequence m_randomSequence;

    /// Static singleton instance.
    static AASaver *m_instance;

public:
    /// Construct the screensaver with window id \p id.
    AASaver(WId id);

    static AASaver *instance();

    /**
     * Returns a QString holding a color mask, created by choosing random colors
     * to replace numbers in \p color_mask.
     */
    QString randColor(QString color_mask);

    double doubleRand(int max);

    /// Adds the castle sprite to the screen.
    void addCastle();

    /// Adds the environment (sea, etc.) to the screen.
    void addEnvironment();

    /// Adds the seaweed to the screen.
    void addAllSeaweed();

    /// Adds the initial layout of fish to the sea, scaling the number of fish
    /// based on the current screen size.
    void addAllFish();

    /**
     * Adds a seaweed to a random position of the sea bottom.
     *
     * @param screen The Screen to add into.
     */
    void addSeaweed(Screen* screen);

    /**
     * Returns a new fish sprite, which has not yet been added to a screen.
     *
     * @param screen The Screen to use when constructing the Sprite.
     * @todo Combine with addFish().
     */
    Sprite *newFish(Screen *screen);

    /**
     * Adds a new fish sprite to \p screen.
     *
     * @param screen The Screen to add a fish to.
     */
    void addFish(Screen *screen);

    /**
     * Adds a new air bubble sprite to \p screen.  The \p x, \p y, and \p z
     * coordinates are all in logical coordinates.
     *
     * @param screen The Screen to add the bubble to.
     * @param x The x position to start the bubble at.
     * @param y The y position to start the bubble at.
     * @param z The z position to start the bubble at.
     */
    void addBubble(Screen* screen, int x, int y, int z);

    /**
     * Adds a Nessie, the Loch Ness Monster sprite to \p screen.
     *
     * @param screen The Screen to add Nessie to.
     */
    void addNessie(Screen* screen);

    /**
     * Adds a big fish sprite to \p screen.
     *
     * @param screen The Screen to add the big fish to.
     */
    void addBigFish(Screen* screen);

    /**
     * Adds a whale sprite to \p screen.
     *
     * @param screen The Screen to add the whale to.
     */
    void addWhale(Screen* screen);

    /**
     * Adds a shark sprite to \p screen.  The shark can kill() fish it comes in
     * contact with (they will spawn more fish automatically).
     *
     * @param screen The Screen to add the shark to.
     */
    void addShark(Screen* screen);

    /**
     * Adds a submarine sprite to \p screen.  The submarine can kill() fish it
     * comes in contact with (they will spawn more fish automatically).
     *
     * @param screen The Screen to add the submarine to.
     * @author Ryan Meldrum <ryjame@cox.net>
     * @since KDE SC 4.6
     */
    void addSubmarine(Screen* screen);

    /**
     * Adds a ship sprite to \p screen.
     *
     * @param screen The Screen to add the ship to.
     */
    void addShip(Screen* screen);

    /**
     * Adds a random object from the set (Shark, Big Fish, Nessie, Whale, Ship, Sub)
     * to the sea.
     *
     * @param screen The Screen to add to.
     */
    void addRandom(Screen* screen);

    /**
     * Reimplemented to update the widget when it gets dirty.
     */
    virtual void paintEvent(QPaintEvent* pe);
};

class AASaverInterface : public KScreenSaverInterface
{
    public:
    virtual ~AASaverInterface();

    virtual KAboutData *aboutData();
    virtual KScreenSaver *create(WId id);
    virtual QDialog *setup();
};

#endif /* AA_AASAVER_H */

// vim: set et ts=8 sw=4:
