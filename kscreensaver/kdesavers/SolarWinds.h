/*
 *  Terence Welsh Screensaver - Solar Winds
 *  http://www.reallyslick.com/
 *
 *  Ported to KDE by Karl Robillard
 *  Copyright (C) 2002  Terence M. Welsh
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of 
 *  the License, or (at your option) any later version.
 *   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLARWINDS_H
#define SOLARWINDS_H

#include <qgl.h>


#define LIGHTSIZE   64


class wind;
class QTimer;

class SWindsWidget : public QGLWidget
{
    Q_OBJECT

public:

    enum eDefault
    {
        Regular,
        CosmicStrings,
        ColdPricklies,
        SpaceFur,
        Jiggly,
        Undertow,

        DefaultModes
    };

    SWindsWidget( QWidget* parent=0 );
    ~SWindsWidget();

    void updateParameters();
    void setDefaults( int which );

protected:

    void paintGL();
    void resizeGL( int w, int h );
    void initializeGL();
#ifdef UNIT_TEST
    void keyPressEvent( QKeyEvent* );
#endif

private slots:

    void nextFrame();

private:

    wind* _winds;
    unsigned char lightTexture[LIGHTSIZE][LIGHTSIZE];

    int dWinds;
    int dEmitters;
    int dParticles;
    int dGeometry;
    float dSize;
    int dParticlespeed;
    int dEmitterspeed;
    int dWindspeed;
    int dBlur;


    // Using QTimer rather than timerEvent() to avoid getting locked out of
    // the QEvent loop on lower-end systems.  Ian Geiser <geiseri@kde.org>
    // says this is the way to go.
    QTimer* _timer;
    int _frameTime;

    friend class wind;
};


#ifndef UNIT_TEST
#include <kdialog.h>
#include <kscreensaver.h>


class KSWindsScreenSaver : public KScreenSaver
{
    Q_OBJECT

public:

    KSWindsScreenSaver( WId id );
    virtual ~KSWindsScreenSaver();

    int mode() const { return _mode; }

public slots:

    void setMode( int );

private:

    void readSettings();

    SWindsWidget* _flux;
    int _mode;
};


class QComboBox;

class KSWindsSetup : public KDialog
{
    Q_OBJECT

public:

    KSWindsSetup( QWidget* parent = 0 );
    ~KSWindsSetup();

private slots:

    void slotHelp();
    void slotOk();

private:

    QComboBox* modeW;
    KSWindsScreenSaver* _saver;
};
#endif


#endif //__SOLARWINDS_H__
