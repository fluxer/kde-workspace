///-----------------------------------------------------------------------------
//
// kgravity - Partical gravity Screen Saver for KDE 2
//
// Copyright (c)  Ian Reinhart Geiser 2001
//
/////
//NOTE:
// The base particle engine did not come from me, it was designed by Jeff Molofee <nehe@connect.ab.ca>
// I did some extensive modifications to make it work with QT's OpenGL but the base principal is about
// the same.
////

#ifndef GRAVITY_H
#define GRAVITY_H

#include <kdialog.h>
#include <qgl.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <kscreensaver.h>
#include <qtimer.h>
#include <qimage.h>
#include "ui_gravitycfg.h"
#include <kcomponentdata.h>
#include <qfile.h>
#include <qtextstream.h>

#define	MAX_PARTICLES	100


class Gravity : public QGLWidget
{
Q_OBJECT
	class particles						// Create A Structure For Particle
	{
	public:
		bool	active;					// Active (Yes/No)
		float	life;					// Particle Life
		float	fade;					// Fade Speed
		float	r;					// Red Value
		float	g;					// Green Value
		float	b;					// Blue Value
		float	x;					// X Position
		float	y;					// Y Position
		float	z;					// Z Position
		float	xo;					// X Position
		float	yo;					// Y Position
		float	zo;					// Z Position
		float	index;					// Index
		float	indexo;
		float	size;					// Particle Size
	};

public:
	Gravity( QWidget * parent=0 );
	~Gravity();
	void setSize( float newSize );
	void setStars( bool doStars );
protected:
	/** paint the GL view */
	void paintGL ();
	/** resize the gl view */
	void resizeGL ( int w, int h );
	/** setup the GL environment */
	void initializeGL ();
	void buildParticle(int loop);

private:
	/** load the partical file */
	bool loadParticle();

	particles particle[MAX_PARTICLES];


	bool	rainbow;					// Rainbow Mode?
	bool	sp;						// Spacebar Pressed?
	bool	rp;						// Enter Key Pressed?
	float	slowdown;					// Slow Down Particles
	float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
	float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)
	float	zoom;					// Used To Zoom Out
	float	size;
	float	stars;
	GLuint	loop;						// Misc Loop Variable
	GLuint	col;						// Current Color Selection
	GLuint	delay;						// Rainbow Effect Delay
	GLuint	texture[1];
	QImage  tex;
	float	index;
	float	transIndex;
	GLfloat scale;
	GLUquadricObj *obj;
};

class KGravitySaver : public KScreenSaver
{
Q_OBJECT
public:
	KGravitySaver( WId drawable );
	virtual ~KGravitySaver();
	void readSettings();
public slots:
	void blank();
	void updateSize(int newSize);
	void doStars(bool starState);
//	void loadTextures(bool textures);
private:
	Gravity *gravity;
	QTimer  *timer;
};

class GravityWidget : public QWidget, public Ui::GravityWidget
{
public:
    GravityWidget( QWidget *parent  ) : QWidget( parent ) {
        setupUi( parent );
    }
};

class KGravitySetup : public KDialog
{
	Q_OBJECT
public:
	KGravitySetup( QWidget *parent = 0L);
        ~KGravitySetup();

protected:
	void readSettings();

private slots:
	void slotOkPressed();
	void aboutPressed();
private:
	KGravitySaver *saver;
	float	size;
	float	stars;
	float	zoom;
	float	speed;
    GravityWidget *cfg;
};

#endif


