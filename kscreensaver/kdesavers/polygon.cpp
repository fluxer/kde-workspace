//-----------------------------------------------------------------------------
//
// kpolygon - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// 2001/03/04 Converted to libkscreensaver by Martin R. Jones

#include <stdlib.h>
#include <time.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qslider.h>
#include <qlayout.h>
#include <QPolygon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include "polygon.h"
#include <qpainter.h>

#include "polygon.moc"


#define MAXLENGTH	65
#define MAXVERTICES	19

// libkscreensaver interface
class KPolygonSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return new KAboutData( "kpolygon.kss", "klock", ki18n( "KPolygon" ), "2.2.0", ki18n( "KPolygon" ) );
    }


    virtual KScreenSaver* create( WId id )
    {
        return new kPolygonSaver( id );
    }

    virtual QDialog* setup()
    {
        return new kPolygonSetup();
    }
};

int main( int argc, char *argv[] )
{
    KPolygonSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
kPolygonSetup::kPolygonSetup( QWidget *parent )
	: KDialog( parent)
	  , saver( 0 ), length( 10 ), vertices( 3 ),
	  speed( 50 )
{
	setCaption(i18n( "Setup Polygon Screen Saver" ));
	setButtons(Ok|Cancel|Help);
	setDefaultButton(Ok);
	setModal(true);
	readSettings();

	QWidget *main = new QWidget(this);
	setMainWidget(main);
	setButtonText( Help, i18n( "A&bout" ) );

	QHBoxLayout *tl = new QHBoxLayout(main);
        tl->setSpacing(spacingHint());
	QVBoxLayout *tl1 = new QVBoxLayout;
	tl->addLayout(tl1);

	QLabel *label = new QLabel( i18n("Length:"), main );
	tl1->addWidget(label);

        QSlider *sb = new QSlider(Qt::Horizontal, main);
        sb->setMinimum(1);
        sb->setMaximum(MAXLENGTH);
        sb->setPageStep(10);
        sb->setValue(length);
        sb->setMinimumSize( 90, 20 );
        sb->setTickPosition(QSlider::TicksBelow);
        sb->setTickInterval(10);
        connect( sb, SIGNAL(valueChanged(int)), SLOT(slotLength(int)) );
        tl1->addWidget(sb);

	label = new QLabel( i18n("Vertices:"), main );
	tl1->addWidget(label);

        sb = new QSlider(Qt::Horizontal, main);
        sb->setMinimum(3);
        sb->setMaximum(MAXVERTICES);
        sb->setPageStep(2);
        sb->setValue(vertices);
        sb->setMinimumSize( 90, 20 );
        sb->setTickPosition(QSlider::TicksBelow);
        sb->setTickInterval(2);
        connect( sb, SIGNAL(valueChanged(int)), SLOT(slotVertices(int)) );
        tl1->addWidget(sb);

	label = new QLabel( i18n("Speed:"), main );
	tl1->addWidget(label);

        sb = new QSlider(Qt::Horizontal, main);
        sb->setMinimum(0);
        sb->setMaximum(100);
        sb->setPageStep(10);
        sb->setValue(speed);
        sb->setMinimumSize( 90, 20 );
        sb->setTickPosition(QSlider::TicksBelow);
        sb->setTickInterval(10);
        connect( sb, SIGNAL(valueChanged(int)), SLOT(slotSpeed(int)) );
        tl1->addWidget(sb);
        tl1->addStretch();

	preview = new QWidget( main );
	preview->setFixedSize( 220, 170 );
	preview->show();    // otherwise saver does not get correct size
	saver = new kPolygonSaver( preview->winId() );
	tl->addWidget(preview);
        connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
	connect(this,SIGNAL(helpClicked()),this,SLOT(slotHelp()));
	setMinimumSize( sizeHint() );
}

kPolygonSetup::~kPolygonSetup()
{
    delete saver;
}

// read settings from config file
void kPolygonSetup::readSettings()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    length = config.readEntry( "Length", length );
    if ( length > MAXLENGTH )
        length = MAXLENGTH;
    else if ( length < 1 )
        length = 1;

    vertices = config.readEntry( "Vertices", vertices );
    if ( vertices > MAXVERTICES )
        vertices = MAXVERTICES;
    else if ( vertices < 3 )
        vertices = 3;

    speed = config.readEntry( "Speed", speed );
    if ( speed > 100 )
        speed = 100;
    else if ( speed < 50 )
        speed = 50;
}

void kPolygonSetup::slotLength( int len )
{
	length = len;
	if ( saver )
		saver->setPolygon( length, vertices );
}

void kPolygonSetup::slotVertices( int num )
{
	vertices = num;
	if ( saver )
		saver->setPolygon( length, vertices );
}

void kPolygonSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( speed );
}

// Ok pressed - save settings and exit
void kPolygonSetup::slotOk()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    QString slength;
    slength.setNum( length );
    config.writeEntry( "Length", slength );

    QString svertices;
    svertices.setNum( vertices );
    config.writeEntry( "Vertices", svertices );

    QString sspeed;
    sspeed.setNum( speed );
    config.writeEntry( "Speed", sspeed );

    config.sync();

    accept();
}

void kPolygonSetup::slotHelp()
{
	KMessageBox::information(this,
			     i18n("Polygon Version 2.2.0\n\n"\
					       "Written by Martin R. Jones 1996\n"\
					       "mjones@kde.org"));
}

//-----------------------------------------------------------------------------


kPolygonSaver::kPolygonSaver( WId id ) : KScreenSaver( id )
{
	readSettings();

	directions.resize( numVertices );

	initialiseColor();
	initialisePolygons();

	timer.start( speed );
	connect( &timer, SIGNAL(timeout()), SLOT(update()) );
        setAttribute( Qt::WA_NoSystemBackground );
        cleared = false;
        show();
}

kPolygonSaver::~kPolygonSaver()
{
	timer.stop();
}

// set polygon properties
void kPolygonSaver::setPolygon( int len, int ver )
{
	timer.stop();
	numLines = len;
	numVertices = ver;

	directions.resize( numVertices );
	polygons.clear();
	initialisePolygons();

	timer.start( speed );
}

// set the speed
void kPolygonSaver::setSpeed( int spd )
{
	timer.stop();
	speed = 100-spd;
	timer.start( speed );
}

// read configuration settings from config file
void kPolygonSaver::readSettings()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    numLines = config.readEntry( "Length", 10 );
    if ( numLines > 50 )
	    numLines = 50;
    else if ( numLines < 1 )
	    numLines = 1;

    numVertices = config.readEntry( "Vertices", 3 );
    if ( numVertices > 20 )
	    numVertices = 20;
    else if ( numVertices < 3 )
	    numVertices = 3;

    speed = 100 - config.readEntry( "Speed", 50 );
}

// draw next polygon and erase tail
void kPolygonSaver::paintEvent(QPaintEvent *)
{
    QPainter p( this );
    if (!cleared) {
        cleared = true;
        p.fillRect(rect(), Qt::black);
    }

	if ( polygons.count() > numLines )
	{
		p.setPen( Qt::black );
                p.drawPolyline( polygons.first() );
	}

	nextColor();
    p.setPen( colors[currentColor] );
    p.drawPolyline( polygons.last() );

	if ( polygons.count() > numLines )
		polygons.removeFirst();

	polygons.append( QPolygon( polygons.last() ) );
	moveVertices();
}

// initialise the polygon
void kPolygonSaver::initialisePolygons()
{
	int i;

	polygons.append( QPolygon( numVertices + 1 ) );

	QPolygon &poly = polygons.last();

	for ( i = 0; i < numVertices; i++ )
	{
		poly.setPoint( i, rnd.getLong(width()), rnd.getLong(height()) );
		directions[i].setX( 16 - rnd.getLong(8) * 4 );
		if ( directions[i].x() == 0 )
			directions[i].setX( 1 );
		directions[i].setY( 16 - rnd.getLong(8) * 4 );
		if ( directions[i].y() == 0 )
			directions[i].setY( 1 );
	}

	poly.setPoint( i, poly.point(0) );
}

// move polygon in current direction and change direction if a border is hit
void kPolygonSaver::moveVertices()
{
	int i;
	QPolygon &poly = polygons.last();

	for ( i = 0; i < numVertices; i++ )
	{
		poly.setPoint( i, poly.point(i) + directions[i] );
		if ( poly[i].x() >= (int)width() )
		{
			directions[i].setX( -(rnd.getLong(4) + 1) * 4 );
			poly[i].setX( (int)width() );
		}
		else if ( poly[i].x() < 0 )
		{
			directions[i].setX( (rnd.getLong(4) + 1) * 4 );
			poly[i].setX( 0 );
		}

		if ( poly[i].y() >= (int)height() )
		{
			directions[i].setY( -(rnd.getLong(4) + 1) * 4 );
			poly[i].setY( height() );
		}
		else if ( poly[i].y() < 0 )
		{
			directions[i].setY( (rnd.getLong(4) + 1) * 4 );
			poly[i].setY( 0 );
		}
	}

	poly.setPoint( i, poly.point(0) );
}

// create a color table of 64 colors
void kPolygonSaver::initialiseColor()
{
	for ( int i = 0; i < 64; i++ )
	{
		colors[i].setHsv( i * 360 / 64, 255, 255 );
	}

    currentColor = 0;
}

// set foreground color to next in the table
void kPolygonSaver::nextColor()
{
	currentColor++;

	if ( currentColor > 63 )
		currentColor = 0;
}

