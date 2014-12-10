/*
 *  Terence Welsh Screensaver - Euphoria
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

#ifndef EUPHORIASS_H
#define EUPHORIASS_H

#include <qgl.h>

#include <kdialog.h>

class wisp;
class QTimer;

class EuphoriaWidget : public QGLWidget
{
    Q_OBJECT

public:

    enum eDefault
    {
        Regular,
        Grid,
        Cubism,
        BadMath,
        MTheory,
        UHFTEM,   // ultra high frequency tunneling electron microscope
        Nowhere,
        Echo,
        Kaleidoscope,
        DefaultModes
    };

    EuphoriaWidget( QWidget* parent=0 );
    ~EuphoriaWidget();

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

    GLuint texName;
    wisp* _wisps;
    wisp* _backwisps;
    unsigned char* feedbackmap;
    float aspectRatio;
    int viewport[4];
    double elapsedTime;

    // feedback texture object
    unsigned int feedbacktex;
    int feedbacktexsize;
    // feedback variables
    float fr[4];
    float fv[4];
    float f[4];
    // feedback limiters
    float lr[3];
    float lv[3];
    float l[3];



    int dWisps;
    int dBackground;
    int dDensity;
    int dVisibility;
    float dSpeed;
    int dFeedback;
    int dFeedbackspeed;
    int dFeedbacksize;
    int dWireframe;
    int dTexture;
    int dPriority;


    // Using QTimer rather than timerEvent() to avoid getting locked out of
    // the QEvent loop on lower-end systems.  Ian Geiser <geiseri@kde.org>
    // says this is the way to go.
    QTimer* _timer;
    int _frameTime;

    friend class wisp;
};


#ifndef UNIT_TEST
#include <qdialog.h>
#include <kscreensaver.h>


class KEuphoriaScreenSaver : public KScreenSaver
{
    Q_OBJECT

public:

    KEuphoriaScreenSaver( WId id );
    virtual ~KEuphoriaScreenSaver();

    int mode() const { return _mode; }

public slots:

    void setMode( int );

private:

    void readSettings();

    EuphoriaWidget* _effect;
    int _mode;
};


class QComboBox;

class KEuphoriaSetup : public KDialog
{
    Q_OBJECT

public:
    KEuphoriaSetup( QWidget* parent = 0 );
    ~KEuphoriaSetup();

private slots:
    void slotHelp();
    void slotOk();

private:
    QComboBox* modeW;
    KEuphoriaScreenSaver* _saver;
};
#endif

#endif //__EUPHORIASS_H__
