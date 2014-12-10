//-----------------------------------------------------------------------------
//
// kwave - Waveing Flag Screen Saver for KDE
//
// Copyright (c)  Ian Reinhart Geiser 2001
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
#include "wave.h"
#include "wave.moc"
#ifdef Q_WS_MACX
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
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
#ifndef GLU_NURBS_TEXTURE_COORD
#define GLU_NURBS_TEXTURE_COORD            100168
#endif
// libkscreensaver interface

class KWaveSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return new KAboutData( "kwave.kss", "klock", ki18n( "Bitmap Wave Screen Saver" ), "2.2.0", ki18n( "Bitmap Wave Screen Saver" ) );
    }


	virtual KScreenSaver* create( WId id )
	{
		return new KWaveSaver( id );
	}

	virtual QDialog* setup()
	{
		return new KWaveSetup();
	}
};

int main( int argc, char *argv[] )
{
    KWaveSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KWaveSetup::KWaveSetup( QWidget *parent)
	: KDialog( parent )
{
	setModal(true);
        setCaption(i18n( "Bitmap Flag Setup" ));
        setButtons(Ok|Cancel|Help);
        setDefaultButton(Ok);
        setButtonText( Help, i18n( "A&bout" ) );
        QWidget *main = new QWidget(this);
        setMainWidget(main);
        cfg = new WaveWidget(main);
	readSettings();

	cfg->preview->setFixedSize( 220, 170 );

        QPalette palette;
        palette.setColor( cfg->preview->backgroundRole(), Qt::black );
        cfg->preview->setPalette( palette );
	cfg->preview->setAutoFillBackground(true);

	cfg->preview->show();    // otherwise saver does not get correct size
	saver = new KWaveSaver( cfg->preview->winId() );

	connect( this, SIGNAL(okClicked()), SLOT(slotOkPressed()) );
	connect( this, SIGNAL(helpClicked()), SLOT(aboutPressed()) );
	connect(  cfg->SpinBox1, SIGNAL(valueChanged(int)), saver, SLOT(updateSize(int)));
	connect( cfg->RadioButton1, SIGNAL(toggled(bool)), saver, SLOT(doStars(bool)));

}

KWaveSetup::~KWaveSetup( )
{
    delete saver;
}


// read settings from config file
void KWaveSetup::readSettings()
{
	KConfigGroup config(KGlobal::config(), "Settings");

//	color = group.configColorEntry( "Color", &black );
}

// Ok pressed - save settings and exit
void KWaveSetup::slotOkPressed()
{
	KConfigGroup config(KGlobal::config(), "Settings");

//	group.writeEntry( "Color", color );

	config.sync();

	accept();
}

void KWaveSetup::aboutPressed()
{
    KMessageBox::about(this,
        i18n("<h3>Bitmap Flag Screen Saver</h3>\n<p>Waving Flag Screen Saver for KDE</p>\nCopyright (c)  Ian Reinhart Geiser 2001"));
}
//-----------------------------------------------------------------------------


KWaveSaver::KWaveSaver( WId id ) : KScreenSaver( id )
{
	kDebug() << "Blank";
	readSettings();

	timer = new QTimer( this );
        timer->setSingleShot( true );
	timer->start( 50 );
        {
            QPalette palette;
            palette.setColor( backgroundRole(), Qt::black );
            setPalette( palette );
	    setAutoFillBackground(true);
        }
	wave = new Wave();
	embed(wave);
	wave->show();
	connect( timer, SIGNAL(timeout()), this, SLOT(blank()) );;
}

KWaveSaver::~KWaveSaver()
{

}

// read configuration settings from config file
void KWaveSaver::readSettings()
{
	KConfigGroup config(KGlobal::config(), "Settings");

//	color = group.configColorEntry( "Color", &black );
}

void KWaveSaver::blank()
{
	// Play wave

	wave->updateGL();
	timer->start( 100 );

}
Wave::Wave( QWidget * parent ) : QGLWidget (parent)
{
	pNurb = 0;

	nNumPoints = 4;
	index = 0;
}

Wave::~Wave()
{
	glDeleteTextures( 1, &texture[0] );
	gluDeleteNurbsRenderer(pNurb);
}

/** setup the GL environment */
void Wave::initializeGL ()
{

	kDebug() << "InitGL";

	/* Load in the texture */
	if ( !LoadGLTextures( ) )
		exit(0);

	/* Enable Texture Mapping ( NEW ) */
	glEnable( GL_TEXTURE_2D );

	/* Enable smooth shading */
	glShadeModel( GL_SMOOTH );
	// Light values and coordinates
	GLfloat  specular[] = { 0.7f, 0.0f, 0.0f, 1.0f};
	GLfloat  shine[] = { 75.0f };

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);

	glEnable(GL_AUTO_NORMAL);


	pNurb = gluNewNurbsRenderer();

	gluNurbsProperty(pNurb, GLU_SAMPLING_TOLERANCE, 25.0f);
		// Uncomment the next line and comment the one following to produce a
		// wire frame mesh.
	//gluNurbsProperty(pNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
	gluNurbsProperty(pNurb, GLU_DISPLAY_MODE, (GLfloat)GLU_FILL);
	glEnable(GL_MAP2_TEXTURE_COORD_3);
    	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_BLEND);

}
/** resize the gl view */
void Wave::resizeGL ( int w, int h)
{
	kDebug() << "ResizeGL " << w << "," <<h;
        // Prevent a divide by zero
        if(h == 0)
                h = 1;

        // Set Viewport to window dimensions
    glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Perspective view
        gluPerspective (45.0f, (GLdouble)w/(GLdouble)h, 1.0, 40.0f);

        // Modelview matrix reset
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Viewing transformation, position for better view
    glTranslatef (0.0f, 0.0f, -20.0f);

}
/** paint the GL view */
void Wave::paintGL ()
{

	float ctrlPoints[4][4][3];

	index++;
	float Z[16];
	for( int i = 0; i < 16; i++)
	{
		Z[i] = 3.0 * sin(16*(3.141592654 * 2.0f) * (index+(i))/360);
	}

//	kDebug() << "-----";
	int counter =15;
	for( int i = 0; i < 4; i++)
		for( int j = 0; j < 4;j++)
		{
			ctrlPoints[i][j][0] = float((5*i)-10);
			ctrlPoints[i][j][1] = float((3*j)-6);
			ctrlPoints[i][j][2] = Z[counter--];
//			kDebug() << Z[counter];
		}
	// Knot sequence for the NURB
	float knots[8] = { 0, 0, 0, 0,  1, 1, 1, 1 };
        // Draw in Blu
        //glColor3ub(0,0,220);

        // Clear the window with current clearing color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Save the modelview matrix stack
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // Rotate the mesh around to make it easier to see
        glRotatef(index/2, 1.0f,0.0f,0.0f);
	glRotatef(index/5, 0.0f,1.0f,0.0f);
	glRotatef(index/6, 0.0f,0.0f,1.0f);

        // Render the NURB
   	gluBeginSurface( pNurb );
		gluNurbsSurface( pNurb, 8, knots, 8, knots,
		4*3, 3, &ctrlPoints[0][0][0], 4, 4, GL_MAP2_TEXTURE_COORD_3);
		//gluNurbsSurface( pNurb, 8, knots, 8, knots,
		//4*3, 3, &ctrlPoints[0][0][0], 4, 4, GL_MAP2_NORMAL );
		gluNurbsSurface( pNurb, 8, knots, 8, knots,
		4*3, 3, &ctrlPoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3 );
   	gluEndSurface( pNurb );

        // Restore the modelview matrix
        glPopMatrix();

	glFlush();
}

bool Wave::LoadGLTextures()
{
    /* Status indicator */
    bool Status = true;

	QImage buf; // = QPixmap::grabWindow ( 0 ).convertToImage();
   kDebug() << "Loading: " << KStandardDirs::locate("data", QLatin1String( "kscreensaver/image.png" ));
 if (buf.load( KStandardDirs::locate("data", QLatin1String( "kscreensaver/image.png" )) ) )

        {
                tex = convertToGLFormat(buf);  // flipped 32bit RGBA
                kDebug() << "Texture loaded: " << tex.numBytes ();
        }
        else
        {
                QImage dummy( 64, 64, QImage::Format_RGB32 );
                dummy.fill( Qt::white );
                buf = dummy;
                tex = convertToGLFormat( buf );
        }
            /* Set the status to true */
            //Status = true;
	glGenTextures(1, &texture[0]);   /* create three textures */
 	glBindTexture(GL_TEXTURE_2D, texture[0]);
	/* use linear filtering */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	/* actually generate the texture */
	glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0,
	GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	kDebug() << "Texture Loaded: " << tex.width() << "," << tex.height();


    return Status;
}
