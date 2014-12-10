//-----------------------------------------------------------------------------
//
// kfountain - Partical Fountain Screen Saver for KDE
//
// Copyright (c)  Ian Reinhart Geiser 2001
//
// KConfig code and KScreenSaver "Setup..." improvements by
// Nick Betcher <nbetcher@usinternet.com> 2001
//
#include <stdlib.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcolordialog.h>
#include <kcolorbutton.h>
#include <kglobal.h>
#include "fountain.h"
#include "fountain.moc"
#ifdef Q_WS_MACX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <qimage.h>
#include <kdebug.h>
#include <qpainter.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <kstandarddirs.h>
#include <math.h>
#include <kmessagebox.h>
#include <krandom.h>
// libkscreensaver interface
class KFountainSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return new KAboutData( "kfountain.kss", "klock", ki18n( "Particle Fountain Screen Saver" ), "2.2.0", ki18n( "Particle Fountain Screen Saver" ) );
    }


	virtual KScreenSaver* create( WId id )
	{
		return new KFountainSaver( id );
	}

	virtual QDialog* setup()
	{
		return new KFountainSetup();
	}
};

int main( int argc, char *argv[] )
{
    KFountainSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KFountainSetup::KFountainSetup( QWidget *parent )
	: KDialog( parent)
{

        setCaption(i18n( "Particle Fountain Setup" ));
        setButtons(Ok|Cancel|Help);
        setDefaultButton(Ok);
        setModal(true);
        setButtonText( Help, i18n( "A&bout" ) );
        QWidget *main = new QWidget(this);
        setMainWidget(main);
        cfg = new FountainWidget(main);

        readSettings();
	cfg->preview->setFixedSize( 220, 170 );
        {
            QPalette palette;
            palette.setColor( cfg->preview->backgroundRole(), Qt::black );
            cfg->preview->setPalette( palette );
	    cfg->preview->setAutoFillBackground(true);
        }
	cfg->preview->show();    // otherwise saver does not get correct size
	saver = new KFountainSaver( cfg->preview->winId() );
	connect( this, SIGNAL(okClicked()), SLOT(slotOkPressed()) );
	connect( this, SIGNAL(helpClicked()), SLOT(aboutPressed()) );
	connect(  cfg->SpinBox1, SIGNAL(valueChanged(int)), saver, SLOT(updateSize(int)));
	connect( cfg->RadioButton1, SIGNAL(toggled(bool)), saver, SLOT(doStars(bool)));

}

KFountainSetup::~KFountainSetup()
{
}

// read settings from config file
void KFountainSetup::readSettings()
{
       KConfig config(QLatin1String( "kssfountainrc" ), KConfig::NoGlobals);
       KConfigGroup grp = config.group("Settings");

	bool boolval = grp.readEntry( "Stars", false );
	if (boolval) {
            cfg->RadioButton1->setChecked(true);
	} else {
            cfg->RadioButton1_2->setChecked(true);
	}

	int starammount = grp.readEntry("StarSize", 75);
	cfg->SpinBox1->setValue(starammount);

}

// Ok pressed - save settings and exit
void KFountainSetup::slotOkPressed()
{
	KConfig _config(QLatin1String( "kssfountainrc" ), KConfig::NoGlobals);
	KConfigGroup config(&_config, "Settings" );

	if (cfg->RadioButton1->isChecked() == true)
	{
		config.writeEntry( "Stars", true );
	} else {
		if (cfg->RadioButton1_2->isChecked() == true)
		{
                    config.writeEntry( "Stars", false );
		}
	}
	config.writeEntry( "StarSize", cfg->SpinBox1->value() );

	config.sync();

	accept();
}

void KFountainSetup::aboutPressed()
{
    KMessageBox::about(this,
        i18n("<h3>Particle Fountain</h3>\n<p>Particle Fountain Screen Saver for KDE</p>\nCopyright (c)  Ian Reinhart Geiser 2001<br>\n\n<p>KConfig code and KScreenSaver \"Setup...\" improvements by Nick Betcher <nbetcher@usinternet.com> 2001</p>"));
}
//-----------------------------------------------------------------------------


KFountainSaver::KFountainSaver( WId id ) : KScreenSaver( id )
{

	kDebug() << "Blank";

	timer = new QTimer( this );
        timer->setSingleShot( true );
        timer->start( 25 );
        {
            QPalette palette;
            palette.setColor( backgroundRole(), Qt::black );
            setPalette( palette );
        }
        update();
	fountain = new Fountain();
	embed(fountain);
	fountain->show();
	connect( timer, SIGNAL(timeout()), this, SLOT(blank()) );
	show();
}

KFountainSaver::~KFountainSaver()
{

}

// read configuration settings from config file
void KFountainSaver::readSettings()
{
// Please remove me

}

void KFountainSaver::blank()
{
	// Play fountain

	fountain->updateGL();
    timer->setSingleShot(true);
	timer->start( 25);

}
Fountain::Fountain( QWidget * parent ) : QGLWidget (parent)
{
	rainbow=true;
	slowdown=2.0f;
	zoom=-40.0f;
	index=0;
	size = 0.75f;
	obj = gluNewQuadric();

// This has to be here because you can't update the fountain until 'fountain' is created!
	KConfig _config(QLatin1String( "kssfountainrc" ), KConfig::NoGlobals);
	KConfigGroup config(&_config, "Settings" );
	bool boolval = config.readEntry( "Stars", false );
        setStars(boolval);

	int starammount = config.readEntry("StarSize", 75);
	float passvalue = (starammount / 100.0);
	setSize(passvalue);

}

Fountain::~Fountain()
{
	glDeleteTextures( 1, &texture[0] );
	gluDeleteQuadric(obj);
}

/** load the particle file */
bool Fountain::loadParticle()
{
    /* Status indicator */
    bool Status = true;
    QImage buf;

    kDebug() << "Loading: " << KStandardDirs::locate("data", QLatin1String( "kscreensaver/particle.png" ));

    if (buf.load( KStandardDirs::locate("data", QLatin1String( "kscreensaver/particle.png" )) ) )
    {
        tex = convertToGLFormat(buf);  // flipped 32bit RGBA
        kDebug() << "Texture loaded: " << tex.numBytes ();
    }
    else
    {
        QImage dummy( 32, 32, QImage::Format_RGB32 );
        dummy.fill( Qt::white );
        buf = dummy;
        tex = convertToGLFormat( buf );
    }

    /* Set the status to true */
    //Status = true;
    glGenTextures(1, &texture[0]);   /* create three textures */
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    /* use linear filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* actually generate the texture */
    glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0,
    GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());



    return Status;
}
/** setup the GL environment */
void Fountain::initializeGL ()
{

	kDebug() << "InitGL";
	GLfloat colors[12][3]=
	{{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}};

	if (loadParticle())						// Jump To Texture Loading Routine
	{
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glDisable( GL_DEPTH_TEST );

    /* Enable Blending */
    glEnable( GL_BLEND );
    /* Type Of Blending To Perform */
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );


    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    /* Really Nice Point Smoothing */
    glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );

    /* Enable Texture Mapping */
    glEnable( GL_TEXTURE_2D );
    /* Select Our Texture */
    glBindTexture( GL_TEXTURE_2D, texture[0] );

		for (loop=0;loop<MAX_PARTICLES;loop++)				// Initials All The Textures
		{
			particle[loop].active=true;				// Make All The Particles Active
			particle[loop].life=1.0f;				// Give All The Particles Full Life
			particle[loop].fade=float(KRandom::random()%100)/1000.0f+0.003f;	// Random Fade Speed
			int color_index = (loop+1)/(MAX_PARTICLES/12);
			color_index = qMin(11, color_index);
			particle[loop].r=colors[color_index][0];	// Select Red Rainbow Color
			particle[loop].g=colors[color_index][1];	// Select Green Rainbow Color
			particle[loop].b=colors[color_index][2];	// Select Blue Rainbow Color
			particle[loop].xi=float((KRandom::random()%50)-26.0f)*10.0f;	// Random Speed On X Axis
			particle[loop].yi=float((KRandom::random()%50)-25.0f)*10.0f;	// Random Speed On Y Axis
			particle[loop].zi=float((KRandom::random()%50)-25.0f)*10.0f;	// Random Speed On Z Axis
			particle[loop].xg=0.0f;					// Set Horizontal Pull To Zero
			particle[loop].yg=-0.8f;				// Set Vertical Pull Downward
			particle[loop].zg=0.0f;					// Set Pull On Z Axis To Zero
			particle[loop].size=size;				// Set particle size.
		}
	}
	else
		exit(0);
}
/** resize the gl view */
void Fountain::resizeGL ( int width, int height )
{
	kDebug() << "ResizeGL " << width << "," <<height;
	if (height==0)							// Prevent A Divide By Zero By
	{
		height=1;						// Making Height Equal One
	}

	glViewport(0,0,width,height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);					// Select The Projection Matrix
	glLoadIdentity();						// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);

	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();
}
/** paint the GL view */
void Fountain::paintGL ()
{
	//kDebug() << "PaintGL";

	GLfloat colors[12][3]=
	{{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}};
	col = ( col + 1 ) % 12;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	glLoadIdentity();
						// Reset The ModelView Matrix
	transIndex++;
	glTranslatef( GLfloat(5.0*sin(4*3.14*transIndex/360)), GLfloat(4.0*cos(2*3.14*transIndex/360)), 0.0 );
	xspeed = GLfloat(100.0*cos(3*3.14*transIndex/360)+100);
	yspeed = GLfloat(100.0*sin(3*3.14*transIndex/360)+100);
	//slowdown = GLfloat(4.0*sin(2*3.14*transIndex/360)+4.01);

	for (loop=0;loop<MAX_PARTICLES;loop++)				// Loop Through All The Particles
	{
		if (particle[loop].active)				// If The Particle Is Active
		{
			float x=particle[loop].x;			// Grab Our Particle X Position
			float y=particle[loop].y;			// Grab Our Particle Y Position
			float z=particle[loop].z+zoom;			// Particle Z Pos + Zoom
    /* Select Our Texture */

                    /* Draw The Particle Using Our RGB Values,
                     * Fade The Particle Based On It's Life
                     */

                    glColor4f( particle[loop].r,
                               particle[loop].g,
                               particle[loop].b,
                               particle[loop].life );

                    /* Build Quad From A Triangle Strip */
		  if( !stars )
		    glBegin( GL_TRIANGLE_STRIP );
		  else
		    glBegin( GL_TRIANGLE_FAN );
                      /* Top Right */
                      glTexCoord2d( 1, 1 );
                      glVertex3f( x + particle[loop].size, y + particle[loop].size, z );
                      /* Top Left */
                      glTexCoord2d( 0, 1 );
                      glVertex3f( x - particle[loop].size, y + particle[loop].size, z );
                      /* Bottom Right */
                      glTexCoord2d( 1, 0 );
                      glVertex3f( x + particle[loop].size, y - particle[loop].size, z );
                      /* Bottom Left */
                      glTexCoord2d( 0, 0 );
                      glVertex3f( x - particle[loop].size, y - particle[loop].size, z );
                    glEnd( );

			particle[loop].x+=particle[loop].xi/(slowdown*1000);// Move On The X Axis By X Speed
			particle[loop].y+=particle[loop].yi/(slowdown*1000);// Move On The Y Axis By Y Speed
			particle[loop].z+=particle[loop].zi/(slowdown*1000);// Move On The Z Axis By Z Speed

			particle[loop].xi+=particle[loop].xg;			// Take Pull On X Axis Into Account
			particle[loop].yi+=particle[loop].yg;			// Take Pull On Y Axis Into Account
			particle[loop].zi+=particle[loop].zg;			// Take Pull On Z Axis Into Account
			particle[loop].life-=particle[loop].fade;		// Reduce Particles Life By 'Fade'

			if (particle[loop].life<0.0f)					// If Particle Is Burned Out
			{
				particle[loop].life=2.0f;				// Give It New Life
				particle[loop].fade=float(KRandom::random()%100)/1000.0f+0.003f;	// Random Fade Value
				particle[loop].x=0.0f;					// Center On X Axis
				particle[loop].y=0.0f;					// Center On Y Axis
				particle[loop].z=0.0f;					// Center On Z Axis
				particle[loop].xi=xspeed+float((KRandom::random()%60)-32.0f);	// X Axis Speed And Direction
				particle[loop].yi=yspeed+float((KRandom::random()%60)-30.0f);	// Y Axis Speed And Direction
				particle[loop].zi=float((KRandom::random()%60)-30.0f);		// Z Axis Speed And Direction
				particle[loop].r=colors[col][0];			// Select Red From Color Table
				particle[loop].g=colors[col][1];			// Select Green From Color Table
				particle[loop].b=colors[col][2];			// Select Blue From Color Table
				particle[loop].size=size;
				if ((1+(random()%20)) == 10)
				{
				// Explode
					particle[loop].active=true;				// Make All The Particles Active
					particle[loop].life=1.0f;				// Give All The Particles Full Life
					particle[loop].fade=float(KRandom::random()%100)/1000.0f+0.003f;	// Random Fade Speed
					int color_index = (loop+1)/(MAX_PARTICLES/12);
					color_index = qMin(11, color_index);
					particle[loop].r=colors[color_index][0];        // Select Red Rainbow Color
					particle[loop].g=colors[color_index][1];        // Select Green Rainbow Color
					particle[loop].b=colors[color_index][2];	// Select Blue Rainbow Color
					particle[loop].xi=float((KRandom::random()%50)-26.0f)*10.0f;	// Random Speed On X Axis
					particle[loop].yi=float((KRandom::random()%50)-25.0f)*10.0f;	// Random Speed On Y Axis
					particle[loop].zi=float((KRandom::random()%50)-25.0f)*10.0f;	// Random Speed On Z Axis
					particle[loop].xg=0.0f;					// Set Horizontal Pull To Zero
					particle[loop].yg=-0.8f;				// Set Vertical Pull Downward
					particle[loop].zg=0.0f;					// Set Pull On Z Axis To Zero
					particle[loop].size=size;				// Set particle size.
				}
			}
			// Lets stir some things up
			index += 0.001;
			particle[loop].yg =2.0*sin(2*3.14*transIndex/360);
			particle[loop].xg =2.0*cos(2*3.14*transIndex/360);
			particle[loop].zg =4.0+(4.0*cos(2*3.14*transIndex/360));

		}
	}

	glFlush();
}
void Fountain::setSize( float newSize )
{
	size = newSize;
}
void Fountain::setStars( bool doStars )
{
	stars = doStars;
}

void KFountainSaver::updateSize(int newSize)
{
	fountain->setSize(newSize/100);
}
void KFountainSaver::doStars(bool starState)
{
	fountain->setStars(starState);
}

