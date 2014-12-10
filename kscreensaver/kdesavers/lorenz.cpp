//-----------------------------------------------------------------------------
//
// Lorenz - Lorenz Attractor screen saver
//   Nicolas Brodu, brodu@kde.org, 2000
//
// Portions of code from kblankscrn and khop.
//   See authors there.
//
// I release my code as GPL, but see the other headers and the README

#include <math.h>
#include <stdlib.h>

#include <qpainter.h>
#include <qslider.h>
#include <qlayout.h>
#include <qcolor.h>
#include <qcolormap.h>
#include <qlabel.h>

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <krandom.h>

#include "lorenz.h"
#include "lorenz.moc"

// libkscreensaver interface
class KLorenzSaverInterface : public KScreenSaverInterface
{


public:
    virtual KAboutData* aboutData() {
        return new KAboutData( "klorenz.kss", "klock", ki18n( "KLorenz" ), "2.2.0", ki18n( "KLorenz" ) );
    }


    virtual KScreenSaver* create( WId id )
    {
        return new KLorenzSaver( id );
    }

    virtual QDialog* setup()
    {
        return new KLorenzSetup();
    }
};

int main( int argc, char *argv[] )
{
    KLorenzSaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}

#define MINSPEED 1
#define MAXSPEED 1500
#define DEFSPEED 150
#define MINZROT -180
#define MAXZROT 180
#define DEFZROT 104 //100
#define MINYROT -180
#define MAXYROT 180
#define DEFYROT -19 //80
#define MINXROT -180
#define MAXXROT 180
#define DEFXROT 25 //20
#define MINEPOCH 1
#define MAXEPOCH 30000
#define DEFEPOCH 5800
#define MINCOLOR 1
#define MAXCOLOR 100
#define DEFCOLOR 20

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KLorenzSetup::KLorenzSetup( QWidget *parent )
    : KDialog( parent)
{
	setCaption(i18n( "Setup Lorenz Attractor" ));
	setButtons(Ok|Cancel|Default|Help);
	setDefaultButton(Ok);
    setModal(true);
	readSettings();

    setButtonText( Help, i18n( "A&bout" ) );
    QWidget *main = new QWidget(this);
	setMainWidget(main);

    QHBoxLayout *tl = new QHBoxLayout( main );
    tl->setSpacing( spacingHint() );
    QVBoxLayout *tl1 = new QVBoxLayout;
    tl->addLayout(tl1);

    QLabel *label = new QLabel( i18n("Speed:"), main );
    tl1->addWidget(label);

    sps = new QSlider(Qt::Horizontal, main);
    sps->setMinimum(MINSPEED);
    sps->setMaximum(MAXSPEED);
    sps->setPageStep(10);
    sps->setValue(speed);
    sps->setMinimumSize( 120, 20 );
    sps->setTickPosition(QSlider::TicksBelow);
    sps->setTickInterval(150);
    connect( sps, SIGNAL(valueChanged(int)), SLOT(slotSpeed(int)) );
    tl1->addWidget(sps);

    label = new QLabel( i18n("Epoch:"), main );
    tl1->addWidget(label);

    eps = new QSlider(Qt::Horizontal, main);
    eps->setMinimum(MINEPOCH);
    eps->setMaximum(MAXEPOCH);
    eps->setPageStep(100);
    eps->setValue(epoch);
    eps->setMinimumSize( 120, 20 );
    eps->setTickPosition(QSlider::TicksBelow);
    eps->setTickInterval(3000);
    connect( eps, SIGNAL(valueChanged(int)), SLOT(slotEpoch(int)) );
    tl1->addWidget(eps);

    label = new QLabel( i18n("Color rate:"), main );
    tl1->addWidget(label);

    crs = new QSlider(Qt::Horizontal, main);
    crs->setMinimum(MINCOLOR);
    crs->setMaximum(MAXCOLOR);
    crs->setPageStep(5);
    crs->setValue(crate);
    crs->setMinimumSize( 120, 20 );
    crs->setTickPosition(QSlider::TicksBelow);
    crs->setTickInterval(10);
    connect( crs, SIGNAL(valueChanged(int)), SLOT(slotCRate(int)) );
    tl1->addWidget(crs);

    label = new QLabel( i18n("Rotation Z:"), main );
    tl1->addWidget(label);

    zrs = new QSlider(Qt::Horizontal, main);
    zrs->setMinimum(MINZROT);
    zrs->setMaximum(MAXZROT);
    zrs->setPageStep(18);
    zrs->setValue(zrot);
    zrs->setMinimumSize( 120, 20 );
    zrs->setTickPosition(QSlider::TicksBelow);
    zrs->setTickInterval(36);
    connect( zrs, SIGNAL(valueChanged(int)), SLOT(slotZRot(int)) );
    tl1->addWidget(zrs);

    label = new QLabel( i18n("Rotation Y:"), main );
    tl1->addWidget(label);

    yrs = new QSlider(Qt::Horizontal, main);
    yrs->setMinimum(MINYROT);
    yrs->setMaximum(MAXYROT);
    yrs->setPageStep(18);
    yrs->setValue(yrot);
    yrs->setMinimumSize( 120, 20 );
    yrs->setTickPosition(QSlider::TicksBelow);
    yrs->setTickInterval(36);
    connect( yrs, SIGNAL(valueChanged(int)), SLOT(slotYRot(int)) );
    tl1->addWidget(yrs);

    label = new QLabel( i18n("Rotation X:"), main );
    tl1->addWidget(label);

    xrs = new QSlider(Qt::Horizontal, main);
    xrs->setMinimum(MINXROT);
    xrs->setMaximum(MAXXROT);
    xrs->setPageStep(18);
    xrs->setValue(xrot);
    xrs->setMinimumSize( 120, 20 );
    xrs->setTickPosition(QSlider::TicksBelow);
    xrs->setTickInterval(36);
    connect( xrs, SIGNAL(valueChanged(int)), SLOT(slotXRot(int)) );
    tl1->addWidget(xrs);

    preview = new QWidget( main );
    preview->setFixedSize( 220, 165 );
    {
        QPalette palette;
        palette.setColor( preview->backgroundRole(), Qt::black );
        preview->setPalette( palette );
	preview->setAutoFillBackground(true);
    }
    preview->show();    // otherwise saver does not get correct size
    saver = new KLorenzSaver( preview->winId() );
    tl->addWidget(preview);
    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
    connect(this,SIGNAL(helpClicked()),this,SLOT(slotHelp()));
    connect(this,SIGNAL(defaultClicked()),this,SLOT(slotDefault()));
}

KLorenzSetup::~KLorenzSetup()
{
    delete saver;
}

// read settings from config file
void KLorenzSetup::readSettings()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    speed = config.readEntry( "Speed", DEFSPEED );
    epoch = config.readEntry( "Epoch", DEFEPOCH );
    crate = config.readEntry( "Color Rate", DEFCOLOR );
    zrot = config.readEntry( "ZRot", DEFZROT );
    yrot = config.readEntry( "YRot", DEFZROT );
    xrot = config.readEntry( "XRot", DEFZROT );
}


void KLorenzSetup::slotSpeed(int num)
{
    speed = num;
    if (saver) saver->setSpeed(speed);
}

void KLorenzSetup::slotEpoch(int num)
{
    epoch = num;
    if (saver) saver->setEpoch(epoch);
}

void KLorenzSetup::slotCRate(int num)
{
    crate = num;
    if (saver) saver->setCRate(crate);
}

void KLorenzSetup::slotZRot(int num)
{
    zrot = num;
    if (saver) {
        saver->setZRot(zrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotYRot(int num)
{
    yrot = num;
    if (saver) {
        saver->setYRot(yrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotXRot(int num)
{
    xrot = num;
    if (saver) {
        saver->setXRot(xrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
}

void KLorenzSetup::slotHelp()
{
    KMessageBox::about(this,i18n("Lorenz Attractor screen saver for KDE\n\nCopyright (c) 2000 Nicolas Brodu"));
}

// Ok pressed - save settings and exit
void KLorenzSetup::slotOk()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    config.writeEntry( "Speed", speed );
    config.writeEntry( "Epoch", epoch );
    config.writeEntry( "Color Rate", crate );
    config.writeEntry( "ZRot", zrot );
    config.writeEntry( "YRot", yrot );
    config.writeEntry( "XRot", xrot );

    config.sync();

    accept();
}

void KLorenzSetup::slotDefault()
{
    speed = DEFSPEED;
    epoch = DEFEPOCH;
    crate = DEFCOLOR;
    zrot = DEFZROT;
    yrot = DEFYROT;
    xrot = DEFXROT;
    if (saver) {
        saver->setSpeed(speed);
        saver->setEpoch(epoch);
        saver->setCRate(crate);
        saver->setZRot(zrot);
        saver->setYRot(yrot);
        saver->setXRot(xrot);
        saver->updateMatrix();
        saver->newEpoch();
    }
    sps->setValue(speed);
    eps->setValue(epoch);
    crs->setValue(crate);
    zrs->setValue(zrot);
    yrs->setValue(yrot);
    xrs->setValue(xrot);

/*  // User can cancel, or save defaults?

    KSharedConfig::Ptr config = KGlobal::config();
    config.setGroup( "Settings" );

    config.writeEntry( "Speed", speed );
    config.writeEntry( "Epoch", epoch );
    config.writeEntry( "Color Rate", crate );
    config.writeEntry( "ZRot", zrot );
    config.writeEntry( "YRot", yrot );
    config.writeEntry( "XRot", xrot );

    config.sync();
*/
}

//-----------------------------------------------------------------------------


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const double pi = M_PI;

// Homogeneous coordinate transform matrix
// I initially wrote it for a Java applet, it is inspired from a
// Matrix class in the JDK.
// Nicolas Brodu, 1998-2000
class Matrix3D
{
    // All coefficients
    double xx, xy, xz, xo;
    double yx, yy, yz, yo;
    double zx, zy, zz, zo;
    // 0, 0, 0, 1 are implicit
public:

    void unit()
    {
        xx=1.0; xy=0.0; xz=0.0; xo=0.0;
        yx=0.0; yy=1.0; yz=0.0; yo=0.0;
        zx=0.0; zy=0.0; zz=1.0; zo=0.0;
    }

    Matrix3D ()
    {
        unit();
    }

    // Translation
    void translate(double x, double y, double z)
    {
        xo += x;
        yo += y;
        zo += z;
    }

    // Rotation, in degrees, around the Y axis
    void rotY(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nxx = xx * ct + zx * st;
        double Nxy = xy * ct + zy * st;
        double Nxz = xz * ct + zz * st;
        double Nxo = xo * ct + zo * st;

        double Nzx = zx * ct - xx * st;
        double Nzy = zy * ct - xy * st;
        double Nzz = zz * ct - xz * st;
        double Nzo = zo * ct - xo * st;

        xo = Nxo;
        xx = Nxx;
        xy = Nxy;
        xz = Nxz;
        zo = Nzo;
        zx = Nzx;
        zy = Nzy;
        zz = Nzz;
    }


    // Rotation, in degrees, around the X axis
    void rotX(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nyx = yx * ct + zx * st;
        double Nyy = yy * ct + zy * st;
        double Nyz = yz * ct + zz * st;
        double Nyo = yo * ct + zo * st;

        double Nzx = zx * ct - yx * st;
        double Nzy = zy * ct - yy * st;
        double Nzz = zz * ct - yz * st;
        double Nzo = zo * ct - yo * st;

        yo = Nyo;
        yx = Nyx;
        yy = Nyy;
        yz = Nyz;
        zo = Nzo;
        zx = Nzx;
        zy = Nzy;
        zz = Nzz;
    }


    // Rotation, in degrees, around the Z axis
    void rotZ(double theta)
    {
        theta *= pi / 180;
        double ct = cos(theta);
        double st = sin(theta);

        double Nyx = yx * ct + xx * st;
        double Nyy = yy * ct + xy * st;
        double Nyz = yz * ct + xz * st;
        double Nyo = yo * ct + xo * st;

        double Nxx = xx * ct - yx * st;
        double Nxy = xy * ct - yy * st;
        double Nxz = xz * ct - yz * st;
        double Nxo = xo * ct - yo * st;

        yo = Nyo;
        yx = Nyx;
        yy = Nyy;
        yz = Nyz;
        xo = Nxo;
        xx = Nxx;
        xy = Nxy;
        xz = Nxz;
    }

    // Multiply by a projection matrix, with camera f
    // f 0 0 0   x   f*x
    // 0 f 0 0 * y = f*y
    // 0 0 1 f   z   z+f
    // 0 0 0 1   1   1
    // So, it it easy to find the 2D coordinates after the transform
    //  u = f*x / (z+f)
    //  v = f*y / (z+f)
    void proj(double f)
    {
        xx*=f;
        xy*=f;
        xz*=f;
        xo*=f;
        yx*=f;
        yy*=f;
        yz*=f;
        yo*=f;
        zo+=f;
    }

    // Apply the transformation 3D => 2D
    void transform(double x, double y, double z, double &u, double& v, double& w)
    {
        u = x * xx + y * xy + z * xz + xo;
        v = x * yx + y * yy + z * yz + yo;
        w = x * zx + y * zy + z * zz + zo;
    }
};

KLorenzSaver::KLorenzSaver( WId id ) : KScreenSaver( id )
{
    readSettings();

    // Create a transform matrix with the parameters
    mat = new Matrix3D();
    updateMatrix();

    newEpoch();

    timer.start( 10 );
    connect( &timer, SIGNAL(timeout()), SLOT(update()) );
    setAttribute( Qt::WA_NoSystemBackground );
    show();
}

KLorenzSaver::~KLorenzSaver()
{
    delete mat;
    mat=0;
    timer.stop();
}

// read configuration settings from config file
void KLorenzSaver::readSettings()
{
    KConfigGroup config(KGlobal::config(), "Settings");

    speed = config.readEntry( "Speed", DEFSPEED );
    epoch = config.readEntry( "Epoch", DEFEPOCH );
    zrot = config.readEntry( "ZRot", DEFZROT );
    yrot = config.readEntry( "YRot", DEFZROT );
    xrot = config.readEntry( "XRot", DEFZROT );

    int crate_num = config.readEntry( "Color Rate", DEFCOLOR );
    crate = (double)crate_num / (double)MAXCOLOR;
}

void KLorenzSaver::setSpeed(int num)
{
    speed = num;
}

void KLorenzSaver::setEpoch(int num)
{
    epoch = num;
}

void KLorenzSaver::setZRot(int num)
{
    zrot = num;
}

void KLorenzSaver::setYRot(int num)
{
    yrot = num;
}

void KLorenzSaver::setXRot(int num)
{
    xrot = num;
}

void KLorenzSaver::setCRate(int num)
{
    crate = (double)num / (double)MAXCOLOR;
}

void KLorenzSaver::updateMatrix()
{
    // reset matrix
    mat->unit();
    // Remove the mean before the rotations...
    mat->translate(-0.95413, -0.96740, -23.60065);
    mat->rotZ(zrot);
    mat->rotY(yrot);
    mat->rotX(xrot);
    mat->translate(0, 0, 100);
    mat->proj(1);
}

void KLorenzSaver::newEpoch()
{
    // Start at a random position, somewhere around the mean
    x = 0.95-25.0+50.0*KRandom::random() / (RAND_MAX+1.0);
    y = 0.97-25.0+50.0*KRandom::random() / (RAND_MAX+1.0);
    z = 23.6-25.0+50.0*KRandom::random() / (RAND_MAX+1.0);
    // start at some random 'time' as well to have different colors
    t = 10000.0*KRandom::random() / (RAND_MAX+1.0);
    e=0; // reset epoch counter
}

// Computes the derivatives using Lorenz equations
static void lorenz(double x, double y, double z, double& dx, double& dy, double& dz)
{
    dx = 10*(y-x);
    dy = 28*x - y - x*z;
    dz = x*y - z*8.0/3.0;
}

// Use a simple Runge-Kutta formula to draw a few points
// No need to go beyond 2nd order for a screensaver!
void KLorenzSaver::paintEvent(QPaintEvent *)
{
    double kx, ky, kz, dx, dy, dz;
    const double h = 0.0001;
    const double tqh = h * 3.0 / 4.0;
    QPainter p(this);

    if ( !e )
        p.fillRect(rect(), Qt::black);

    QColormap cmap = QColormap::instance();
    for (int i=0; i<speed; i++) {
        // Runge-Kutta formula
        lorenz(x,y,z,dx,dy,dz);
        lorenz(x + tqh*dx, y + tqh*dy, z + tqh*dz, kx, ky, kz);
        x += h*(dx/3.0+2*kx/3.0);
        y += h*(dy/3.0+2*ky/3.0);
        z += h*(dz/3.0+2*kz/3.0);
        // Apply transform
        mat->transform(x,y,z,kx,ky,kz);
        // Choose a color
        p.setPen(
            cmap.pixel(QColor((int)(sin(t*crate/pi)*127+128),
                              (int)(sin(t*crate/(pi-1))*127+128),
                              (int)(sin(t*crate/(pi-2))*127+128))) );
        // Draw a point
        p.drawPoint( (int)(kx*width()*1.5/kz)+(int)(width()/2),
                     (int)(ky*height()*1.5/kz)+(int)(height()/2));
        t+=h;
    }
    if (++e>=epoch)
        newEpoch();
}
