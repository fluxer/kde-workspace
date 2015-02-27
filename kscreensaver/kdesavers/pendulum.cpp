/** @file
 *
 * KPendulum screen saver for KDE
 *
 * The screen saver displays a physically realistic simulation of a two-part
 * pendulum.
 *
 * Copyright (C) 2004 Georg Drenkhahn, Georg.Drenkhahn@gmx.net
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License or (at your option) version 3 or
 * any later version accepted by the membership of KDE e.V. (or its successor
 * approved by the membership of KDE e.V.), which shall act as a proxy defined
 * in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 */

#define QT_NO_COMPAT

// std. C++ headers
#include <cstdlib>

// Qt headers
#include <QLineEdit>
#include <QSpinBox>
#include <QValidator>
#include <QColorDialog>
#include <QPushButton>
#include <QToolTip>
#include <QResizeEvent>

// KDE headers
#include <KLocale>
#include <KGlobal>
#include <KConfig>
#include <KDebug>
#include <KMessageBox>

// Eigen2 from KDE support
#include <Eigen/Core>
#include <Eigen/Geometry>
// import most common Eigen types
using namespace Eigen;

// the screen saver preview area class
#include "sspreviewarea.h"

#include "pendulum.h"           // own interfaces
#include <kcolordialog.h>
#include "moc_pendulum.cpp"

#define KPENDULUM_VERSION "2.0"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// libkscreensaver interface
class KPendulumSaverInterface : public KScreenSaverInterface
{
   public:
      /// aboutdata instance for libkscreensaver interface
      virtual KAboutData* aboutData()
      {
         return new KAboutData(
            "kpendulum.kss", 0,
            ki18n("Simulation of a two-part pendulum"),
            KPENDULUM_VERSION,
            ki18n("Simulation of a two-part pendulum"));
      }

      /// function to create screen saver object
      virtual KScreenSaver* create(WId id)
      {
         return new KPendulumSaver(id);
      }

      /// function to create setup dialog for screen saver
      virtual QDialog* setup()
      {
         return new KPendulumSetup();
      }
};

int main( int argc, char *argv[] )
{
   KPendulumSaverInterface kss;
   return kScreenSaverMain(argc, argv, kss);
}

//-----------------------------------------------------------------------------
// PendulumOdeSolver
//-----------------------------------------------------------------------------

PendulumOdeSolver::PendulumOdeSolver(
   const double&   t,
   const double&   dt,
   const Vector4d& y,
   const double&   eps,
   const double&   m1,
   const double&   m2,
   const double&   l1,
   const double&   l2,
   const double&   g)
   : RkOdeSolver<double,4>(t, y, dt, eps),
     // constants for faster numeric calculation, derived from m1,m2,l1,l2,g
     m_A(1.0/(m2*l1*l1)),
     m_B1(m2*l1*l2),
     m_B(1.0/m_B1),
     m_C((m1+m2)/(m2*m2*l2*l2)),
     m_D(g*(m1+m2)*l1),
     m_E(g*m2*l2),
     m_M((m1+m2)/m2)
{
}

Vector4d PendulumOdeSolver::f(const double& x, const Vector4d& y) const
{
   (void)x; // unused

   const double& q1 = y[0];
   const double& q2 = y[1];
   const double& p1 = y[2];
   const double& p2 = y[3];

   const double cosDq = std::cos(q1-q2);
   const double iden  = 1.0/(m_M - cosDq*cosDq); // invers denominator
   const double dq1dt = (m_A*p1 - m_B*cosDq*p2)*iden;
   const double dq2dt = (m_C*p2 - m_B*cosDq*p1)*iden;

   Vector4d ypr;
   ypr[0] = dq1dt;
   ypr[1] = dq2dt;

   const double K = m_B1 * dq1dt*dq2dt * std::sin(q1-q2);
   ypr[2] = -K - m_D * std::sin(q1);
   ypr[3] =  K - m_E * std::sin(q2);

   return ypr;
}

//-----------------------------------------------------------------------------
// Rotation: screen saver widget
//-----------------------------------------------------------------------------

PendulumGLWidget::PendulumGLWidget(QWidget* parent)
   : QGLWidget(parent),
     m_eyeR(30),                  // eye coordinates (polar)
     m_eyeTheta(M_PI*0.45),
     m_eyePhi(0),
     m_lightR(m_eyeR),              // light coordinates (polar)
     m_lightTheta(M_PI*0.25),
     m_lightPhi(M_PI*0.25),
     m_quadM1(gluNewQuadric()),
     m_barColor(KPendulumSaver::sm_barColorDefault),
     m_m1Color(KPendulumSaver::sm_m1ColorDefault),
     m_m2Color(KPendulumSaver::sm_m2ColorDefault)
{
}

PendulumGLWidget::~PendulumGLWidget(void)
{
   gluDeleteQuadric(m_quadM1);
}

void PendulumGLWidget::setEyePhi(double phi)
{
   m_eyePhi = phi;
   while (m_eyePhi < 0)
   {
      m_eyePhi += 2.*M_PI;
   }
   while (m_eyePhi > 2*M_PI)
   {
      m_eyePhi -= 2.*M_PI;
   }

   // get the view port
   GLint vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);

   // calc new perspective, a resize event is simulated here
   resizeGL(static_cast<int>(vp[2]), static_cast<int>(vp[3]));
}

void PendulumGLWidget::setAngles(const double& q1, const double& q2)
{
   m_ang1 = static_cast<GLfloat>(q1*180./M_PI);
   m_ang2 = static_cast<GLfloat>(q2*180./M_PI);
}

void PendulumGLWidget::setMasses(const double& m1, const double& m2)
{
   m_sqrtm1 = static_cast<GLfloat>(std::sqrt(m1));
   m_sqrtm2 = static_cast<GLfloat>(std::sqrt(m2));
}

void PendulumGLWidget::setLengths(const double& l1, const double& l2)
{
   m_l1 = static_cast<GLfloat>(l1);
   m_l2 = static_cast<GLfloat>(l2);
}

void PendulumGLWidget::setBarColor(const QColor& c)
{
   if (c.isValid())
   {
      m_barColor = c;
   }
}

void PendulumGLWidget::setM1Color(const QColor& c)
{
   if (c.isValid())
   {
      m_m1Color = c;
   }
}
void PendulumGLWidget::setM2Color(const QColor& c)
{
   if (c.isValid())
   {
      m_m2Color = c;
   }
}

/* --------- protected methods ----------- */

void PendulumGLWidget::initializeGL(void)
{
   qglClearColor(QColor(Qt::black)); // set color to clear the background

   glClearDepth(1);             // depth buffer setup
   glEnable(GL_DEPTH_TEST);     // depth testing
   glDepthFunc(GL_LEQUAL);      // type of depth test

   glShadeModel(GL_SMOOTH);     // smooth color shading in poygons

   // nice perspective calculation
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   // set up the light
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix
   glLoadIdentity();
   // set position of light0
   GLfloat lightPos[4]=
      {GLfloat(m_lightR * std::sin(m_lightTheta) * std::sin(m_lightPhi)),
       GLfloat(m_lightR * std::sin(m_lightTheta) * std::cos(m_lightPhi)),
       GLfloat(m_lightR * std::cos(m_lightTheta)),
       0};
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
   // set position of light1
   lightPos[0] = m_lightR * std::sin(m_lightTheta) * std::sin(m_lightPhi+M_PI);
   lightPos[1] = m_lightR * std::sin(m_lightTheta) * std::cos(m_lightPhi+M_PI);
   glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

   // only for lights #>0
   GLfloat spec[] = {1,1,1,1};
   glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, spec);

   // enable setting the material colour by glColor()
   glEnable(GL_COLOR_MATERIAL);

   GLfloat emi[4] = {.13, .13, .13, 1};
   glMaterialfv(GL_FRONT, GL_EMISSION, emi);
}

void PendulumGLWidget::paintGL(void)
{
   // clear color and depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix

   glLoadIdentity();

   const GLfloat width     = 2.0;
   const GLfloat masswidth = 1.0;
   const int noOfSlices    = 20;

   // top axis, left (x>0)
   glTranslatef(0.5*width, 0, 0);
   glRotatef(90, 0, 1, 0);
   qglColor(m_barColor);
   gluCylinder(m_quadM1, 0.2, 0.2, 5, 10, 1);
   gluSphere(m_quadM1, 0.2, 10, 10);
   // top axis, right
   glLoadIdentity();
   glTranslatef(-0.5*width, 0, 0);
   glRotatef(-90, 0, 1, 0);
   gluCylinder(m_quadM1, 0.2, 0.2, 5, 10, 1);
   gluSphere(m_quadM1, 0.2, 10, 10);
   // 1. part, left
   glLoadIdentity();
   glRotatef(m_ang1, 1, 0, 0);
   glPushMatrix();
   glTranslatef(0.5*width, 0, -m_l1);
   gluCylinder(m_quadM1, 0.2, 0.2, m_l1, 10, 1);
   glPopMatrix();

   // 1. part, right
   glPushMatrix();
   glTranslatef(-0.5*width, 0, -m_l1);
   gluCylinder(m_quadM1, 0.2, 0.2, m_l1, 10, 1);
   // 1. part, bottom
   glRotatef(90, 0, 1, 0);
   gluSphere(m_quadM1, 0.2, 10, 10); // bottom corner 1
   gluCylinder(m_quadM1, 0.2, 0.2, width, 10, 1); // connection
   glTranslatef(0, 0, 0.5*(width-masswidth));
   qglColor(m_m1Color);
   gluCylinder(m_quadM1, m_sqrtm1, m_sqrtm1, masswidth, noOfSlices, 1); // mass 1
   gluQuadricOrientation(m_quadM1, GLU_INSIDE);
   gluDisk(m_quadM1, 0, m_sqrtm1, noOfSlices, 1); // bottom of mass
   gluQuadricOrientation(m_quadM1, GLU_OUTSIDE);
   glTranslatef(0, 0, masswidth);
   gluDisk(m_quadM1, 0, m_sqrtm1, noOfSlices, 1); // top of mass

   glTranslatef(0, 0, 0.5*(width-masswidth));
   qglColor(m_barColor);
   gluSphere(m_quadM1, 0.2, 10, 10); // bottom corner 2
   glPopMatrix();

   // 2. pendulum bar
   glLoadIdentity();
   glTranslatef(0, m_l1*std::sin(m_ang1*M_PI/180.), -m_l1*std::cos(m_ang1*M_PI/180.));
   glRotatef(m_ang2, 1, 0, 0);
   glTranslatef(0, 0, -m_l2);
   qglColor(m_barColor);
   gluCylinder(m_quadM1, 0.2, 0.2, m_l2, 10, 1);

   // mass 2
   glRotatef(90, 0, 1, 0);
   glTranslatef(0, 0, -0.5*masswidth);
   qglColor(m_m2Color);
   gluCylinder(m_quadM1, m_sqrtm2, m_sqrtm2, masswidth, noOfSlices, 1);
   gluQuadricOrientation(m_quadM1, GLU_INSIDE);
   gluDisk(m_quadM1, 0, m_sqrtm2, noOfSlices, 1); // bottom of mass
   gluQuadricOrientation(m_quadM1, GLU_OUTSIDE);
   glTranslatef(0, 0, masswidth);
   gluDisk(m_quadM1, 0, m_sqrtm2, noOfSlices, 1); // top of mass

   glFlush();
}

void PendulumGLWidget::resizeGL(int w, int h)
{
   kDebug() << "w=" << w << ", h=" << h << "\n";

   // prevent a divide by zero
   if (h == 0)
   {
      return;
   }

   // set the new view port
   glViewport(0, 0, static_cast<GLint>(w), static_cast<GLint>(h));

   // set up projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // Perspective view
   gluPerspective(
      40.0f,
      static_cast<GLdouble>(w)/static_cast<GLdouble>(h),
      1.0, 100.0f);

   // Viewing transformation, position for better view
   // Theta is polar angle 0<Theta<Pi
   gluLookAt(
      m_eyeR * std::sin(m_eyeTheta) * std::sin(m_eyePhi),
      m_eyeR * std::sin(m_eyeTheta) * std::cos(m_eyePhi),
      m_eyeR * std::cos(m_eyeTheta),
      0,0,0,
      0,0,1);
}

//-----------------------------------------------------------------------------
// KPendulumSaver: screen saver class
//-----------------------------------------------------------------------------

// class methods

KPendulumSaver::KPendulumSaver(WId id) :
   KScreenSaver(id),
   m_solver(0),
   m_massRatio(sm_massRatioDefault),
   m_lengthRatio(sm_lengthRatioDefault),
   m_g(sm_gDefault),
   m_E(sm_EDefault),
   m_persChangeInterval(sm_persChangeIntervalDefault)
{
   // no need to set our parent widget's background here, the GL widget sets its
   // own background color


   m_glArea = new PendulumGLWidget(); // create gl widget w/o parent
   m_glArea->setEyePhi(sm_eyePhiDefault);
   readSettings();              // read global settings into pars


   // init m_glArea with read settings, construct and init m_solver
   initData();

   embed(m_glArea);               // embed gl widget and resize it
   m_glArea->show();              // show embedded gl widget

   // set up and start cyclic timer
   m_timer = new QTimer(this);
   m_timer->start(sm_deltaT);
   connect(m_timer, SIGNAL(timeout()), this, SLOT(doTimeStep()));
}

KPendulumSaver::~KPendulumSaver()
{
   m_timer->stop();

   // m_timer is automatically deleted with parent KPendulumSaver
   delete m_solver;
   delete m_glArea;
}


void KPendulumSaver::readSettings()
{
   // read configuration settings from config file
   KConfigGroup config(KGlobal::config(), "Settings");

   // internal saver parameters are set to stored values or left at their
   // default values if stored values are out of range
   setMassRatio(
      config.readEntry(
         "mass ratio",
         KPendulumSaver::sm_massRatioDefault));
   setLengthRatio(
      config.readEntry(
         "length ratio",
         KPendulumSaver::sm_lengthRatioDefault));
   setG(
      config.readEntry(
         "g",
         KPendulumSaver::sm_gDefault));
   setE(
      config.readEntry(
         "E",
         KPendulumSaver::sm_EDefault));
   setPersChangeInterval(
      config.readEntry(
         "perspective change interval",
         (uint)KPendulumSaver::sm_persChangeIntervalDefault));

   // set the colours
   setBarColor(config.readEntry("bar color", sm_barColorDefault));
   setM1Color( config.readEntry("m1 color",  sm_m1ColorDefault));
   setM2Color( config.readEntry("m2 color",  sm_m2ColorDefault));
}

void KPendulumSaver::initData()
{
   const double m1plusm2 = 2;   // m1+m2
   const double m2       = m_massRatio * m1plusm2;
   const double m1       = m1plusm2 - m2;
   m_glArea->setMasses(m1, m2);
   m_glArea->setAngles(0, 0);

   const double l1plusl2 = 9;   // l1+l2
   const double l2       = m_lengthRatio * l1plusl2;
   const double l1       = l1plusl2 - l2;
   m_glArea->setLengths(l1, l2);

   // kinetic energy of m2 and m1
   const double kin_energy = m_E * m_g * (l1*m1 + (m1+m2)*(l1+l2));
   // angular velocity for 1. and 2. pendulum
   const double qp = std::sqrt(2.*kin_energy/((m1+m2)*l1*l1 + m2*l2*l2 + m2*l1*l2));

   // assemble initial y for solver
   Vector4d y;
   y[0] = 0;                                  // q1
   y[1] = 0;                                  // q2
   y[2] = (m1+m2)*l1*l1*qp + 0.5*m2*l1*l2*qp; // p1
   y[3] = m2*l2*l2*qp + 0.5*m2*l1*l2*qp;      // p2

   // delete old solver
   if (m_solver!=0)
   {
      delete m_solver;
   }
   // init new solver
   m_solver = new PendulumOdeSolver(
      0.0,                      // t
      0.01,                     // first dt step size estimation
      y,
      1e-5,                     // eps
      m1,
      m2,
      l1,
      l2,
      m_g);
}


void KPendulumSaver::setBarColor(const QColor& c)
{
   m_glArea->setBarColor(c);
}
QColor KPendulumSaver::barColor(void) const
{
   return m_glArea->barColor();
}

void KPendulumSaver::setM1Color(const QColor& c)
{
   m_glArea->setM1Color(c);
}
QColor KPendulumSaver::m1Color(void) const
{
   return m_glArea->m1Color();
}

void KPendulumSaver::setM2Color(const QColor& c)
{
   m_glArea->setM2Color(c);
}
QColor KPendulumSaver::m2Color(void) const
{
   return m_glArea->m2Color();
}


void KPendulumSaver::setMassRatio(const double& massRatio)
{
   // range check is not necessary in normal operation because validators check
   // the values at input.  But the validators do not check for corrupted
   // settings read from disk.
   if ((massRatio >= sm_massRatioLimitLower)
       && (massRatio <= sm_massRatioLimitUpper)
       && (m_massRatio != massRatio))
   {
      m_massRatio = massRatio;
      if (m_timer!=0)
      {
         initData();
      }
   }
}

void KPendulumSaver::setLengthRatio(const double& lengthRatio)
{
   if ((lengthRatio >= sm_lengthRatioLimitLower)
       && (lengthRatio <= sm_lengthRatioLimitUpper)
       && (m_lengthRatio != lengthRatio))
   {
      m_lengthRatio = lengthRatio;
      if (m_timer!=0)
      {
         initData();
      }
   }
}

void KPendulumSaver::setG(const double& g)
{
   if ((g >= sm_gLimitLower)
       && (g <= sm_gLimitUpper)
       && (m_g != g))
   {
      m_g = g;
      if (m_timer!=0)
      {
         initData();
      }
   }
}

void KPendulumSaver::setE(const double& E)
{
   if ((E >= sm_ELimitLower)
       && (E <= sm_ELimitUpper)
       && (m_E != E))
   {
      m_E = E;
      if (m_timer!=0)
      {
         initData();
      }
   }
}

void KPendulumSaver::setPersChangeInterval(
   const unsigned int& persChangeInterval)
{
   if ((persChangeInterval >= sm_persChangeIntervalLimitLower)
       && (persChangeInterval <= sm_persChangeIntervalLimitUpper)
       && (m_persChangeInterval != persChangeInterval))
   {
      m_persChangeInterval = persChangeInterval;
      // do not restart simulation here
   }
}

void KPendulumSaver::doTimeStep()
{
   /* time (in seconds) of perspective change.
    * - t<0: no change yet
    * - t=0: change starts
    * - 0<t<moving time: change takes place
    * - t=moving time: end of the change */
   static double persChangeTime = -5;

   // integrate a step ahead
   m_solver->integrate(0.001 * sm_deltaT);

   // read new y from solver
   const Vector4d& y = m_solver->Y();

   // tell glArea the new coordinates/angles of the pendulum
   m_glArea->setAngles(y[0], y[1]);

   // handle perspective change
   persChangeTime += 0.001 * sm_deltaT;
   if (persChangeTime > 0)
   {
      // phi value at the start of a perspective change
      static double eyePhi0     = sm_eyePhiDefault;
      // phi value at the end of a perspective change
      static double eyePhi1     = 0.75*M_PI;
      static double deltaEyePhi = eyePhi1-eyePhi0;

      // movement acceleration/deceleration
      const double a = 3;
      // duration of the change period
      const double movingTime = 2.*std::sqrt(std::abs(deltaEyePhi)/a);

      // new current phi of eye
      double eyePhi = persChangeTime < 0.5*movingTime ?
         // accelerating phase
         eyePhi0 + (deltaEyePhi>0?1:-1)
         * 0.5*a*persChangeTime*persChangeTime:
         // decellerating phase
         eyePhi1 - (deltaEyePhi>0?1:-1)
         * 0.5*a*(movingTime-persChangeTime)*(movingTime-persChangeTime);

      if (persChangeTime > movingTime)
      {  // perspective change has finished
         // set new time till next change
         persChangeTime = -double(m_persChangeInterval);
         eyePhi0 = eyePhi = eyePhi1;
         // find new phi value with angleLimit < phi < Pi-angleLimit or
         // Pi+angleLimit < phi < 2*Pi-angleLimit
         const double angleLimit = M_PI*0.2;
         for (eyePhi1 = 0;
              (eyePhi1<angleLimit)
                 || ((eyePhi1<M_PI+angleLimit) && (eyePhi1>M_PI-angleLimit))
                 || (eyePhi1>2*M_PI-angleLimit);
              eyePhi1 = double(rand())/RAND_MAX * 2*M_PI)
         {
         }
         // new delta phi for next change
         deltaEyePhi = eyePhi1 - eyePhi0;
         // find shortest perspective change
         if (deltaEyePhi < -M_PI)
         {
            deltaEyePhi += 2*M_PI;
         }
      }

      m_glArea->setEyePhi(eyePhi); // set new perspective
   }

   m_glArea->updateGL();          // repaint scenery

   // restarting timer not necessary here, it is a cyclic timer
}

// public slot of KPendulumSaver, forward resize event to public slot of glArea
// to allow the resizing of the gl area withing the setup dialog
void KPendulumSaver::resizeGlArea(QResizeEvent* e)
{
   m_glArea->resize(e->size());
}

// public static class member variables

const QColor KPendulumSaver::sm_barColorDefault(255, 255, 127);
const QColor KPendulumSaver::sm_m1ColorDefault( 170,   0, 127);
const QColor KPendulumSaver::sm_m2ColorDefault(  85, 170, 127);

const double KPendulumSaver::sm_massRatioLimitLower                =   0.01;
const double KPendulumSaver::sm_massRatioLimitUpper                =   0.99;
const double KPendulumSaver::sm_massRatioDefault                   =   0.5;

const double KPendulumSaver::sm_lengthRatioLimitLower              =   0.01;
const double KPendulumSaver::sm_lengthRatioLimitUpper              =   0.99;
const double KPendulumSaver::sm_lengthRatioDefault                 =   0.5;

const double KPendulumSaver::sm_gLimitLower                        =   0.1;
const double KPendulumSaver::sm_gLimitUpper                        = 300.0;
const double KPendulumSaver::sm_gDefault                           =  40.0;

const double KPendulumSaver::sm_ELimitLower                        =   0.0;
const double KPendulumSaver::sm_ELimitUpper                        =   5.0;
const double KPendulumSaver::sm_EDefault                           =   1.2;

const unsigned int KPendulumSaver::sm_persChangeIntervalLimitLower =   5;
const unsigned int KPendulumSaver::sm_persChangeIntervalLimitUpper = 600;
const unsigned int KPendulumSaver::sm_persChangeIntervalDefault    =  15;

// private static class member variables

const unsigned int KPendulumSaver::sm_deltaT                       =  20;
const double KPendulumSaver::sm_eyePhiDefault = 0.25 * M_PI;

//-----------------------------------------------------------------------------
// KPendulumSetup: dialog to setup screen saver parameters
//-----------------------------------------------------------------------------

KPendulumSetup::KPendulumSetup(QWidget* parent)
   : KDialog(parent)
{

   // the dialog should block, no other control center input should be possible
   // until the dialog is closed
   setModal(true);
   setCaption(i18n( "KPendulum Setup" ));
   setButtons(Ok|Cancel|Help);
   setDefaultButton(Ok);
   setButtonText( Help, i18n( "A&bout" ) );
   QWidget *main = new QWidget(this);
   setMainWidget(main);
   cfg = new PendulumWidget();
   cfg->setupUi( main );

   // create input validators
   cfg->m_mEdit->setValidator(
      new QDoubleValidator(
         KPendulumSaver::sm_massRatioLimitLower,
         KPendulumSaver::sm_massRatioLimitUpper,
         5, cfg->m_mEdit));
   cfg->m_lEdit->setValidator(
      new QDoubleValidator(
         KPendulumSaver::sm_lengthRatioLimitLower,
         KPendulumSaver::sm_lengthRatioLimitUpper,
         5, cfg->m_lEdit));
   cfg->m_gEdit->setValidator(
      new QDoubleValidator(
         KPendulumSaver::sm_gLimitLower,
         KPendulumSaver::sm_gLimitUpper,
         5, cfg->m_gEdit));
   cfg->m_eEdit->setValidator(
      new QDoubleValidator(
         KPendulumSaver::sm_ELimitLower,
         KPendulumSaver::sm_ELimitUpper,
         5, cfg->m_eEdit));

   // set input limits for the perspective change interval time
   cfg->m_persSpinBox->setMinimum(KPendulumSaver::sm_persChangeIntervalLimitLower);
   cfg->m_persSpinBox->setMaximum(KPendulumSaver::sm_persChangeIntervalLimitUpper);

   // set tool tips of editable fields
   cfg->m_mEdit->setToolTip(
      ki18n("Ratio of 2nd mass to sum of both masses.\nValid values from %1 to %2.")
      .subs(KPendulumSaver::sm_massRatioLimitLower, 0, 'f', 2)
      .subs(KPendulumSaver::sm_massRatioLimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_lEdit->setToolTip(
      ki18n("Ratio of 2nd pendulum part length to the sum of both part lengths.\nValid values from %1 to %2.")
      .subs(KPendulumSaver::sm_lengthRatioLimitLower, 0, 'f', 2)
      .subs(KPendulumSaver::sm_lengthRatioLimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_gEdit->setToolTip(
      ki18n("Gravitational constant in arbitrary units.\nValid values from %1 to %2.")
      .subs(KPendulumSaver::sm_gLimitLower, 0, 'f', 2)
      .subs(KPendulumSaver::sm_gLimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_eEdit->setToolTip(
      ki18n("Energy in units of the maximum potential energy of the given configuration.\nValid values from %1 to %2.")
      .subs(KPendulumSaver::sm_ELimitLower, 0, 'f', 2)
      .subs(KPendulumSaver::sm_ELimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_persSpinBox->setToolTip(
      ki18n("Time in seconds after which a random perspective change occurs.\nValid values from %1 to %2.")
      .subs(KPendulumSaver::sm_persChangeIntervalLimitLower)
      .subs(KPendulumSaver::sm_persChangeIntervalLimitUpper)
      .toString());

   // init preview area
   QPalette palette;
   palette.setColor(cfg->m_preview->backgroundRole(), Qt::black);
   cfg->m_preview->setPalette(palette);
   cfg->m_preview->setAutoFillBackground(true);
   cfg->m_preview->show();    // otherwise saver does not get correct size

   // create saver and give it the WinID of the preview area
   m_saver = new KPendulumSaver(cfg->m_preview->winId());

   // read settings from saver and update GUI elements with these values, saver
   // has read settings in its constructor

   // set editable fields with stored values as defaults
   QString text;
   text.setNum(m_saver->massRatio());
   cfg->m_mEdit->setText(text);
   text.setNum(m_saver->lengthRatio());
   cfg->m_lEdit->setText(text);
   text.setNum(m_saver->g());
   cfg->m_gEdit->setText(text);
   text.setNum(m_saver->E());
   cfg->m_eEdit->setText(text);

   cfg->m_persSpinBox->setValue(m_saver->persChangeInterval());

   palette.setColor(cfg->m_barColorButton->backgroundRole(), m_saver->barColor());
   cfg->m_barColorButton->setPalette(palette);
   palette.setColor(cfg->m_m1ColorButton->backgroundRole(), m_saver->m1Color());
   cfg->m_m1ColorButton->setPalette(palette);
   palette.setColor(cfg->m_m2ColorButton->backgroundRole(), m_saver->m2Color());
   cfg->m_m2ColorButton->setPalette(palette);

   // if the preview area is resized it emits the resized() event which is
   // caught by m_saver.  The embedded GLArea is resized to fit into the preview
   // area.
   connect(cfg->m_preview, SIGNAL(resized(QResizeEvent*)),
           m_saver,   SLOT(resizeGlArea(QResizeEvent*)));

   connect(this,       SIGNAL(okClicked()),         this, SLOT(okButtonClickedSlot()));
   connect(this,    SIGNAL(helpClicked()),         this, SLOT(aboutButtonClickedSlot()));

   connect(cfg->m_lEdit,          SIGNAL(lostFocus()),       this, SLOT(lEditLostFocusSlot()));
   connect(cfg->m_gEdit,          SIGNAL(lostFocus()),       this, SLOT(gEditLostFocusSlot()));
   connect(cfg->m_eEdit,          SIGNAL(lostFocus()),       this, SLOT(eEditLostFocusSlot()));
   connect(cfg->m_persSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(persChangeEnteredSlot(int)));
   connect(cfg->m_mEdit,          SIGNAL(lostFocus()),       this, SLOT(mEditLostFocusSlot()));
   connect(cfg->m_barColorButton, SIGNAL(clicked()),         this, SLOT(barColorButtonClickedSlot()));
   connect(cfg->m_m1ColorButton,  SIGNAL(clicked()),         this, SLOT(m1ColorButtonClickedSlot()));
   connect(cfg->m_m2ColorButton,  SIGNAL(clicked()),         this, SLOT(m2ColorButtonClickedSlot()));
}

KPendulumSetup::~KPendulumSetup()
{
   delete m_saver;
   delete cfg;
}

// Ok pressed - save settings and exit
void KPendulumSetup::okButtonClickedSlot()
{
   KConfigGroup config(KGlobal::config(), "Settings");

   config.writeEntry("mass ratio",   m_saver->massRatio());
   config.writeEntry("length ratio", m_saver->lengthRatio());
   config.writeEntry("g",            m_saver->g());
   config.writeEntry("E",            m_saver->E());
   config.writeEntry("perspective change interval",
                      m_saver->persChangeInterval());
   config.writeEntry("bar color",    m_saver->barColor());
   config.writeEntry("m1 color",     m_saver->m1Color());
   config.writeEntry("m2 color",     m_saver->m2Color());

   config.sync();
   accept();
}

void KPendulumSetup::aboutButtonClickedSlot()
{
   KMessageBox::about(this, i18n("\
<h3>KPendulum Screen Saver for KDE</h3>\
<p>Simulation of a two-part pendulum</p>\
<p>Copyright (c) Georg&nbsp;Drenkhahn 2004</p>\
<p><tt>Georg.Drenkhahn@gmx.net</tt></p>"));
}

void KPendulumSetup::mEditLostFocusSlot(void)
{
   if (cfg->m_mEdit->hasAcceptableInput())
   {
      m_saver->setMassRatio(cfg->m_mEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(m_saver->massRatio());
      cfg->m_mEdit->setText(text);
   }
}
void KPendulumSetup::lEditLostFocusSlot(void)
{
   if (cfg->m_lEdit->hasAcceptableInput())
   {
      m_saver->setLengthRatio(cfg->m_lEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(m_saver->lengthRatio());
      cfg->m_lEdit->setText(text);
   }
}
void KPendulumSetup::gEditLostFocusSlot(void)
{
   if (cfg->m_gEdit->hasAcceptableInput())
   {
      m_saver->setG(cfg->m_gEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(m_saver->g());
      cfg->m_gEdit->setText(text);
   }
}
void KPendulumSetup::eEditLostFocusSlot(void)
{
   if (cfg->m_eEdit->hasAcceptableInput())
   {
      m_saver->setE(cfg->m_eEdit->text().toDouble());
   }
   else
   {  // write current setting back into input field
      QString text;
      text.setNum(m_saver->E());
      cfg->m_eEdit->setText(text);
   }
}
void KPendulumSetup::persChangeEnteredSlot(int t)
{
   m_saver->setPersChangeInterval(t);
}

void KPendulumSetup::barColorButtonClickedSlot(void)
{
    QColor color = m_saver->barColor();
    if ( KColorDialog::getColor( color, this) )
    {
        if (color.isValid())
        {
            m_saver->setBarColor(color);
            QPalette palette;
            palette.setColor(cfg->m_barColorButton->backgroundRole(), color);
            cfg->m_barColorButton->setPalette(palette);
        }
    }
}

void KPendulumSetup::m1ColorButtonClickedSlot(void)
{
    QColor color =m_saver->m1Color();
    if ( KColorDialog::getColor( color, this) )
    {
        if (color.isValid())
        {
            m_saver->setM1Color(color);
            QPalette palette;
            palette.setColor(cfg->m_m1ColorButton->backgroundRole(), color);
            cfg->m_m1ColorButton->setPalette(palette);
        }
    }
}
void KPendulumSetup::m2ColorButtonClickedSlot(void)
{
    QColor color = m_saver->m2Color();
    if ( KColorDialog::getColor( color, this) )
    {
        if (color.isValid())
        {
            m_saver->setM2Color(color);
            QPalette palette;
            palette.setColor(cfg->m_m2ColorButton->backgroundRole(), color);
            cfg->m_m2ColorButton->setPalette(palette);
        }
    }
}
