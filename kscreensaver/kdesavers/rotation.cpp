/** @file
 *
 * KRotation screen saver for KDE
 *
 * The screen saver displays a physically realistic simulation of a force free
 * rotating asymmetric body.  The equations of motion for such a rotation, the
 * Euler equations, are integrated numerically by the Runge-Kutta method.
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

// STL headers
#include <deque>

// Qt headers
#include <QCheckBox>
#include <QLineEdit>
#include <QValidator>
#include <QToolTip>
#include <QtGui/qevent.h>

// KDE headers
#include <KLocale>
#include <KGlobal>
#include <KConfig>
#include <KDebug>
#include <KMessageBox>

// Eigen2 from KDE support
#include <Eigen/Core>
#include <Eigen/Geometry>
// import all Eigen types, Transform and Quaternion are not part of the
// namespace part published by USING_PART_OF_NAMESPACE_EIGEN
using namespace Eigen;

// the screen saver preview area class
#include "sspreviewarea.h"

#include "rotation.h"           // own interfaces
#include "moc_rotation.cpp"

/** Version number of this screen saver */
#define KROTATION_VERSION "2.0"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// libkscreensaver interface
class KRotationSaverInterface : public KScreenSaverInterface
{
   public:
      virtual KAboutData* aboutData()
      {
         return new KAboutData(
            "krotation.kss", "klock",
            ki18n("Simulation of a force free rotating asymmetric body"),
            KROTATION_VERSION,
            ki18n("Simulation of a force free rotating asymmetric body"));
      }

      /** function to create screen saver object */
      virtual KScreenSaver* create(WId id)
      {
         return new KRotationSaver(id);
      }

      /** function to create setup dialog for screen saver */
      virtual QDialog* setup()
      {
         return new KRotationSetup();
      }
};

int main(int argc, char *argv[])
{
   KRotationSaverInterface kss;
   return kScreenSaverMain(argc, argv, kss);
}

//-----------------------------------------------------------------------------
// EulerOdeSolver implementation
//-----------------------------------------------------------------------------

EulerOdeSolver::EulerOdeSolver(
   const double& t,
   const double& dt,
   const double& A,
   const double& B,
   const double& C,
   Vector12d&    y,
   const double& eps)
   : RkOdeSolver<double,12>(t, y, dt, eps),
     m_A(A),
     m_B(B),
     m_C(C)
{
}

Vector12d EulerOdeSolver::f(const double& x, const Vector12d& y) const
{
   // unused
   (void)x;

   // vec omega in body coor. sys.: omega_body = (p, q, r)
   const Vector3d omega_body(y.head<3>());

   // body unit vectors in fixed frame coordinates
   Matrix3d e;
   for (int i=0; i<3; ++i)
   {
      e.col(i) = y.segment<3>(3+i*3);
   }

   // vec omega in global fixed coor. sys.
   const Vector3d omega = e * omega_body;

   // return vector y'
   Vector12d ypr;

   // omega_body'
   ypr[0] = -(m_C-m_B)/m_A * omega_body[1] * omega_body[2]; // p'
   ypr[1] = -(m_A-m_C)/m_B * omega_body[2] * omega_body[0]; // q'
   ypr[2] = -(m_B-m_A)/m_C * omega_body[0] * omega_body[1]; // r'

   // e1', e2', e3'
   for (int i=0; i<3; ++i)
   {
      ypr.segment<3>(3+i*3) = omega.cross(e.col(i));
   }

   return ypr;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Rotation: screen saver widget
//-----------------------------------------------------------------------------

RotationGLWidget::RotationGLWidget(
   QWidget*              parent,
   const KRotationSaver& saver)
   : QGLWidget(parent),
     m_eyeR(25),
     m_eyeTheta(1),
     m_eyePhi(M_PI*0.25),
     m_boxSize(Vector3d::Ones()),
     m_fixedAxses(0),
     m_bodyAxses(0),
     m_lightR(10),
     m_lightTheta(M_PI/4),
     m_lightPhi(0),
     m_bodyAxsesLength(6),
     m_fixedAxsesLength(8),
     m_saver(saver)
{
   /* Set the box sizes from the momenta of inertia.  J is the 3 vector with
    * momenta of inertia with respect to the 3 figure axes. */

   const Vector3d& J = m_saver.J();

   /* the default values must be valid so that w,h,d are real! */
   const GLfloat x2 = 6.0*(-J[0] + J[1] + J[2]);
   const GLfloat y2 = 6.0*( J[0] - J[1] + J[2]);
   const GLfloat z2 = 6.0*( J[0] + J[1] - J[2]);

   if ((x2>=0) && (y2>=0) && (z2>=0))
   {
      m_boxSize = Vector3d(std::sqrt(x2), std::sqrt(y2), std::sqrt(z2));
   }
   else
   {
      kError() << "parameter error";
   }
}

/* --------- protected methods ----------- */

void RotationGLWidget::initializeGL(void)
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
   // set position of light0
   GLfloat lightPos[4]=
      {m_lightR * std::sin(m_lightTheta) * std::sin(m_lightPhi),
       m_lightR * std::sin(m_lightTheta) * std::cos(m_lightPhi),
       m_lightR * std::cos(m_lightTheta), 1.};
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

   // enable setting the material colour by glColor()
   glEnable(GL_COLOR_MATERIAL);

   // set up display lists

   if (m_fixedAxses == 0)
   {
      m_fixedAxses = glGenLists(1); // list to be returned
   }
   glNewList(m_fixedAxses, GL_COMPILE);

   // fixed coordinate system axes

   glPushMatrix();
   glLoadIdentity();

   // z-axis, blue
   qglColor(QColor(Qt::blue));
   myGlArrow(m_fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   // x-axis, red
   qglColor(QColor(Qt::red));
   glRotatef(90, 0, 1, 0);

   myGlArrow(m_fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   // y-axis, green
   qglColor(QColor(Qt::green));
   glLoadIdentity();
   glRotatef(-90, 1, 0, 0);
   myGlArrow(m_fixedAxsesLength, 0.5f, 0.03f, 0.1f);

   glPopMatrix();
   glEndList();
   // end of axes object list


   // box and box-axses
   if (m_bodyAxses == 0)
   {
      m_bodyAxses = glGenLists(1); // list to be returned
   }
   glNewList(m_bodyAxses, GL_COMPILE);

   // z-axis, blue
   qglColor(QColor(Qt::blue));
   myGlArrow(m_bodyAxsesLength, 0.5f, 0.03f, 0.1f);

   // x-axis, red
   qglColor(QColor(Qt::red));
   glPushMatrix();
   glRotatef(90, 0, 1, 0);
   myGlArrow(m_bodyAxsesLength, 0.5f, 0.03f, 0.1f);
   glPopMatrix();

   // y-axis, green
   qglColor(QColor(Qt::green));
   glPushMatrix();
   glRotatef(-90, 1, 0, 0);
   myGlArrow(m_bodyAxsesLength, 0.5f, 0.03f, 0.1f);
   glPopMatrix();

   glEndList();
}

void RotationGLWidget::draw_traces(void)
{
   const std::deque<Matrix3d>&e = m_saver.e();

   // traces must contain at least 2 elements
   if (e.size() <= 1)
   {
      return;
   }

   glPushMatrix();
   glScalef(m_bodyAxsesLength, m_bodyAxsesLength, m_bodyAxsesLength);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for (int j=0; j<3; ++j)
   {
      if (m_saver.traceFlag(j))
      {
         // emission colour
         GLfloat em[4] = {0,0,0,1};
         em[j] = 1; // set either red, green, blue emission colour

         glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);
         glColor4fv(em);

         // set iterator of the tail part
         std::deque<Matrix3d>::const_iterator eit  = e.begin();
         std::deque<Matrix3d>::const_iterator tail =
            e.begin() +
            static_cast<std::deque<Matrix3d>::difference_type>
            (0.9 * e.size());

         glBegin(GL_LINES);
         for (; eit < e.end()-1; ++eit)
         {
            glVertex3f((*eit)(0,j), (*eit)(1,j), (*eit)(2,j));
            // decrease transparency for tail section
            if (eit > tail)
            {
               em[3] =
                  static_cast<GLfloat>
                  (1.0 - double(eit-tail)/(0.1 * e.size()));
            }
            glColor4fv(em);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);
            glVertex3f((*(eit+1))(0,j), (*(eit+1))(1,j), (*(eit+1))(2,j));
         }
         glEnd();
      }
   }

   glDisable(GL_BLEND);

   glPopMatrix();
}

void RotationGLWidget::paintGL(void)
{
   // clear color and depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);           // select modelview matrix

   glLoadIdentity();
   GLfloat const em[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);

   glPushMatrix();
   // calculate the transform which rotates the unit z vector onto omega
   glLoadMatrixd(
      Projective3d(
         Quaternion<double>()
         .setFromTwoVectors(Vector3d::UnitZ(), m_saver.omega())
         .toRotationMatrix())
      .data());
   // draw the white omega arrow
   qglColor(QColor(Qt::white));
   myGlArrow(7, .5f, .1f, 0.2f);
   glPopMatrix();

   // draw the fixed axes
   glCallList(m_fixedAxses);


   // create transformation/rotation matrix from the body and its unit axes
   glPushMatrix();
   glLoadMatrixd(
      Projective3d(m_saver.e().front())
      .data());

   // draw the body unit axis
   glCallList(m_bodyAxses);

   // scaling from a cube to the rotating body
   glScalef(m_boxSize[0]/2, m_boxSize[1]/2, m_boxSize[2]/2);

   // paint box
   glBegin(GL_QUADS);
   // front (z)
   qglColor(QColor(Qt::blue));
   glNormal3f( 0,0,1);
   glVertex3f( 1,  1,  1);
   glVertex3f(-1,  1,  1);
   glVertex3f(-1, -1,  1);
   glVertex3f( 1, -1,  1);
   // back (-z)
   glNormal3f( 0,0,-1);
   glVertex3f( 1,  1, -1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f( 1, -1, -1);
   // top (y)
   qglColor(QColor(Qt::green));
   glNormal3f( 0,1,0);
   glVertex3f( 1,  1,  1);
   glVertex3f( 1,  1, -1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1,  1,  1);
   // bottom (-y)
   glNormal3f( 0,-1,0);
   glVertex3f( 1, -1,  1);

   glVertex3f( 1, -1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1,  1);
   // left (-x)
   qglColor(QColor(Qt::red));
   glNormal3f( -1,0,0);
   glVertex3f(-1,  1,  1);
   glVertex3f(-1,  1, -1);
   glVertex3f(-1, -1, -1);
   glVertex3f(-1, -1,  1);
   // right (x)
   glNormal3f( 1,0,0);
   glVertex3f( 1,  1,  1);
   glVertex3f( 1,  1, -1);
   glVertex3f( 1, -1, -1);
   glVertex3f( 1, -1,  1);
   glEnd();

   glPopMatrix();

   // draw the traces
   draw_traces();

   glFlush();
}

void RotationGLWidget::resizeGL(int w, int h)
{
   kDebug() << "w=" << w << ", h=" << h << "\n";

   // prevent division by zero
   if (h == 0)
   {
      return;
   }

   // set the new view port
   glViewport(0, 0, (GLint)w, (GLint)h);

   // set up projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // Perspective view
   gluPerspective(40.0f, (GLdouble)w/(GLdouble)h, 1.0, 100.0f);

   // Viewing transformation, position for better view
   // Theta is polar angle 0<Theta<Pi
   gluLookAt(
      m_eyeR * std::sin(m_eyeTheta) * std::sin(m_eyePhi),
      m_eyeR * std::sin(m_eyeTheta) * std::cos(m_eyePhi),
      m_eyeR * std::cos(m_eyeTheta),
      0,0,0,
      0,0,1);
}

/* --------- privat methods ----------- */

void RotationGLWidget::myGlArrow(
   GLfloat total_length, GLfloat head_length,
   GLfloat base_width,   GLfloat head_width)
{
   GLUquadricObj* const quadAx = gluNewQuadric();
   glPushMatrix();
   gluCylinder(
      quadAx, base_width, base_width,
      total_length - head_length, 10, 1);
   glTranslatef(0, 0, total_length - head_length);
   gluCylinder(quadAx, head_width, 0, head_length, 10, 1);
   glPopMatrix();
   gluDeleteQuadric(quadAx);
}


//-----------------------------------------------------------------------------
// KRotationSaver: screen saver class
//-----------------------------------------------------------------------------

// class methods

KRotationSaver::KRotationSaver(WId id)
   : KScreenSaver(id),
     m_solver(0),
     m_glArea(0),
     m_timer(0),
     m_J(4,2,3),                // fixed box sizes!
     m_traceLengthSeconds(sm_traceLengthSecondsDefault),
     m_Lz(sm_LzDefault),
     m_initEulerPhi(0),
     m_initEulerPsi(0),
     m_initEulerTheta(sm_initEulerThetaDefault)
{
   // no need to set our parent widget's background here, the GL widget sets its
   // own background color

   readSettings();              // read global settings

   // init m_e1,m_e2,m_e3,m_omega, construct and init m_solver
   initData();

   // create gl widget w/o parent
   m_glArea = new RotationGLWidget(0, *this);
   embed(m_glArea);             // embed gl widget and resize it
   m_glArea->show();            // show embedded gl widget

   // set up cyclic timer
   m_timer = new QTimer(this);
   m_timer->start(sm_deltaT);
   connect(m_timer, SIGNAL(timeout()), this, SLOT(doTimeStep()));
}

KRotationSaver::~KRotationSaver()
{
   m_timer->stop();

   // m_timer is automatically deleted with parent KRotationSaver
   delete m_glArea;
   delete m_solver;
}

void KRotationSaver::initData()
{
   // rotation by phi around z = zhat axis
   Matrix3d et(AngleAxisd(m_initEulerPhi, Vector3d::UnitZ()));
   // rotation by theta around new x axis
   et = AngleAxisd(m_initEulerPhi, et.col(0)).toRotationMatrix() * et;
   // rotation by psi around new z axis
   et = AngleAxisd(m_initEulerPsi, et.col(2)).toRotationMatrix() * et;

   // set first vector in deque
   m_e.clear(); m_e.push_front(et);

   /* calc L in body frame:
    *
    * determine unit-axes of fixed frame in body coordinates, invert the
    * transformations above for unit vectors of the body frame */

   // rotation by -psi along z axis
   Matrix3d e_body(AngleAxisd(-m_initEulerPsi, Vector3d::UnitZ()));
   // rotation by -theta along new x axis
   e_body = AngleAxisd(-m_initEulerTheta, e_body.col(0)).toRotationMatrix() * e_body;

   // omega_body = L_body * J_body^(-1)
   // component-wise division because J_body is a diagonal matrix
   Vector3d omega_body = (m_Lz * e_body.col(2)).array() / m_J.array();

   // initial rotation vector
   m_omega = et * omega_body;

   // assemble initial y for solver
   Vector12d y;
   y.head<3>() = omega_body;
   // 3 basis vectors of body system in fixed coordinates
   y.segment<3>(3) = et.col(0);
   y.segment<3>(6) = et.col(1);
   y.segment<3>(9) = et.col(2);

   if (m_solver!=0)
   {
      // deleting the solver is necessary when parameters are changed in the
      // configuration dialog.
      delete m_solver;
   }
   // init solver
   m_solver = new EulerOdeSolver(
      0.0,      // t
      0.01,     // first dt step size estimation
      m_J[0], m_J[1], m_J[2], // A,B,C
      y,        // omega_body,e1,e2,e3
      1e-5);    // eps
}

void KRotationSaver::readSettings()
{
   // read configuration settings from config file
   KConfigGroup config(KGlobal::config(), "Settings");

   // internal saver parameters are set to stored values or left at their
   // default values if stored values are out of range
   setTraceFlag(0, config.readEntry("x trace", sm_traceFlagDefault[0]));
   setTraceFlag(1, config.readEntry("y trace", sm_traceFlagDefault[1]));
   setTraceFlag(2, config.readEntry("z trace", sm_traceFlagDefault[2]));
   setRandomTraces(config.readEntry("random traces", sm_randomTracesDefault));
   setTraceLengthSeconds(
      config.readEntry("length", sm_traceLengthSecondsDefault));
   setLz(
      config.readEntry("Lz",     sm_LzDefault));
   setInitEulerTheta(
      config.readEntry("theta",  sm_initEulerThetaDefault));
}

void KRotationSaver::setTraceLengthSeconds(const double& t)
{
   if ((t >= sm_traceLengthSecondsLimitLower)
       && (t <= sm_traceLengthSecondsLimitUpper))
   {
      m_traceLengthSeconds = t;
   }
}

void KRotationSaver::setLz(const double& Lz)
{
   if ((Lz >= sm_LzLimitLower) && (Lz <= sm_LzLimitUpper))
   {
      m_Lz = Lz;
   }
}

void KRotationSaver::setInitEulerTheta(const double& theta)
{
   if ((theta >= sm_initEulerThetaLimitLower)
       && (theta <= sm_initEulerThetaLimitUpper))
   {
      m_initEulerTheta = theta;
   }
}

//   public slots

void KRotationSaver::doTimeStep()
{
   // integrate a step ahead
   m_solver->integrate(0.001*sm_deltaT);

   // read new y
   Vector12d y = m_solver->Y();

   std::deque<Vector3d>::size_type
      max_vec_length =
      static_cast<std::deque<Vector3d>::size_type>
      ( m_traceLengthSeconds/(0.001*sm_deltaT) );

   // construct matrix from solution vector
   // read out new body coordinate system
   Matrix3d et;
   for (int j=0; j<3; ++j)
   {
      et.col(j) = y.segment<3>(3*j+3);
   }

   m_e.push_front(et);

   if (max_vec_length > 0)
   {
      m_e.push_front(et);
      while (m_e.size() > max_vec_length)
      {
         m_e.pop_back();
      }
   }
   else
   {
      // only set the 1. element
      m_e.front() = et;
      // and delete all other emements
      if (m_e.size() > 1)
      {
         m_e.resize(1);
      }
   }

   // current rotation vector omega
   m_omega = m_e.front() * y.head<3>();

   // set new random traces every 10 seconds
   if (m_randomTraces)
   {
      static unsigned int counter=0;
      ++counter;
      if (counter > unsigned(10.0/(0.001*sm_deltaT)))
      {
         counter=0;
         for (int i=0; i<3; ++i)
         {
            m_traceFlag[i] = (rand()%2==1);
         }
      }
   }

   m_glArea->updateGL();

   // no need to restart timer here, it is a cyclic timer
}

// public slot of KRotationSaver, forward resize event to public slot of glArea
// to allow the resizing of the gl area withing the setup dialog
void KRotationSaver::resizeGlArea(QResizeEvent* e)
{
   m_glArea->resize(e->size());
}

// public static class members

const double KRotationSaver::sm_traceLengthSecondsLimitLower =  0.0;
const double KRotationSaver::sm_traceLengthSecondsLimitUpper = 99.0;
const double KRotationSaver::sm_traceLengthSecondsDefault    =  3.0;

const bool   KRotationSaver::sm_traceFlagDefault[3] = {false, false, true};
const bool   KRotationSaver::sm_randomTracesDefault          = true;

const double KRotationSaver::sm_LzLimitLower                 =   0.0;
const double KRotationSaver::sm_LzLimitUpper                 = 500.0;
const double KRotationSaver::sm_LzDefault                    =  10.0;

const double KRotationSaver::sm_initEulerThetaLimitLower     =   0.0;
const double KRotationSaver::sm_initEulerThetaLimitUpper     = 180.0;
const double KRotationSaver::sm_initEulerThetaDefault        =   0.03;

// private static class members

const unsigned int KRotationSaver::sm_deltaT                 = 20;


//-----------------------------------------------------------------------------
// KRotationSetup: dialog to setup screen saver parameters
//-----------------------------------------------------------------------------

KRotationSetup::KRotationSetup(QWidget* parent)
   : KDialog(parent)
{
   setCaption(i18n( "KRotation Setup" ));
   setButtons(Ok|Cancel|Help);
   setDefaultButton(Ok);
   setButtonText( Help, i18n( "A&bout" ) );
   QWidget *main = new QWidget(this);
   setMainWidget(main);
   cfg = new RotationWidget();
   cfg->setupUi( main );

   // the dialog should block, no other control center input should be possible
   // until the dialog is closed
   setModal(true);

   cfg->m_lengthEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::sm_traceLengthSecondsLimitLower,
         KRotationSaver::sm_traceLengthSecondsLimitUpper,
         3, cfg->m_lengthEdit));
   cfg->m_LzEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::sm_LzLimitLower,
         KRotationSaver::sm_LzLimitUpper,
         3, cfg->m_LzEdit));
   cfg->m_thetaEdit->setValidator(
      new QDoubleValidator(
         KRotationSaver::sm_initEulerThetaLimitLower,
         KRotationSaver::sm_initEulerThetaLimitUpper,
         3, cfg->m_thetaEdit));

   // set tool tips of editable fields
   cfg->m_lengthEdit->setToolTip(
      ki18n("Length of traces in seconds of visibility.\nValid values from %1 to %2.")
      .subs(KRotationSaver::sm_traceLengthSecondsLimitLower, 0, 'f', 2)
      .subs(KRotationSaver::sm_traceLengthSecondsLimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_LzEdit->setToolTip(
      ki18n("Angular momentum in z direction in arbitrary units.\nValid values from %1 to %2.")
      .subs(KRotationSaver::sm_LzLimitLower, 0, 'f', 2)
      .subs(KRotationSaver::sm_LzLimitUpper, 0, 'f', 2)
      .toString());
   cfg->m_thetaEdit->setToolTip(
      ki18n("Gravitational constant in arbitrary units.\nValid values from %1 to %2.")
      .subs(KRotationSaver::sm_initEulerThetaLimitLower, 0, 'f', 2)
      .subs(KRotationSaver::sm_initEulerThetaLimitUpper, 0, 'f', 2)
      .toString());

   // setting the background of m_preview widget is not necessary, it's content
   // is overlayed with the embedded GL widget anyway
   cfg->m_preview->show();    // otherwise saver does not get correct size initially

   // create screen saver and give it the WinID of the preview area in which it
   // will be embedded
   m_saver = new KRotationSaver(cfg->m_preview->winId());

   // read settings from saver and update GUI elements with these values, saver
   // has read settings in its constructor

   // set editable fields with stored values as defaults
   cfg->m_xTrace->setChecked(m_saver->traceFlag(0));
   cfg->m_yTrace->setChecked(m_saver->traceFlag(1));
   cfg->m_zTrace->setChecked(m_saver->traceFlag(2));
   cfg->m_randTraces->setChecked(m_saver->randomTraces());
   QString text;
   text.setNum(m_saver->traceLengthSeconds());
   cfg->m_lengthEdit->setText(text);
   text.setNum(m_saver->Lz());
   cfg->m_LzEdit->setText(text);
   text.setNum(m_saver->initEulerTheta());
   cfg->m_thetaEdit->setText(text);

   // if the preview area is resized it emits the resized() event which is
   // caught by m_saver.  The embedded GLArea is resized to fit into the preview
   // area.
   connect(cfg->m_preview,   SIGNAL(resized(QResizeEvent*)),
           m_saver,     SLOT(resizeGlArea(QResizeEvent*)));

   connect(this,    SIGNAL(okClicked()),            this, SLOT(okButtonClickedSlot()));
   connect(this, SIGNAL(helpClicked()),            this, SLOT(aboutButtonClickedSlot()));

   connect(cfg->m_xTrace,      SIGNAL(toggled(bool)),        this, SLOT(xTraceToggled(bool)));
   connect(cfg->m_randTraces,  SIGNAL(toggled(bool)),        this, SLOT(randomTracesToggled(bool)));
   connect(cfg->m_yTrace,      SIGNAL(toggled(bool)),        this, SLOT(yTraceToggled(bool)));
   connect(cfg->m_zTrace,      SIGNAL(toggled(bool)),        this, SLOT(zTraceToggled(bool)));
   connect(cfg->m_lengthEdit,  SIGNAL(textChanged(QString)), this, SLOT(lengthEnteredSlot(QString)));
   connect(cfg->m_LzEdit,      SIGNAL(textChanged(QString)), this, SLOT(LzEnteredSlot(QString)));
   connect(cfg->m_thetaEdit,   SIGNAL(textChanged(QString)), this, SLOT(thetaEnteredSlot(QString)));
}

KRotationSetup::~KRotationSetup()
{
   delete m_saver;
   delete cfg;
}

// Ok pressed - save settings and exit
void KRotationSetup::okButtonClickedSlot(void)
{
   KConfigGroup config(KGlobal::config(), "Settings");
   config.writeEntry("x trace",       m_saver->traceFlag(0));
   config.writeEntry("y trace",       m_saver->traceFlag(1));
   config.writeEntry("z trace",       m_saver->traceFlag(2));
   config.writeEntry("random traces", m_saver->randomTraces());
   config.writeEntry("length",        m_saver->traceLengthSeconds());
   config.writeEntry("Lz",            m_saver->Lz());
   config.writeEntry("theta",         m_saver->initEulerTheta());
   config.sync();
   accept();
}

void KRotationSetup::aboutButtonClickedSlot(void)
{
   KMessageBox::about(this, i18n("\
<h3>KRotation Screen Saver for KDE</h3>\
<p>Simulation of a force free rotating asymmetric body</p>\
<p>Copyright (c) Georg&nbsp;Drenkhahn 2004</p>\
<p><tt>Georg.Drenkhahn@gmx.net</tt></p>"));
}

void KRotationSetup::xTraceToggled(bool state)
{
   m_saver->setTraceFlag(0, state);
}
void KRotationSetup::yTraceToggled(bool state)
{
   m_saver->setTraceFlag(1, state);
}
void KRotationSetup::zTraceToggled(bool state)
{
   m_saver->setTraceFlag(2, state);
}
void KRotationSetup::randomTracesToggled(bool state)
{
   m_saver->setRandomTraces(state);
   if (!state)
   {
      // restore settings from gui if random traces are turned off
      m_saver->setTraceFlag(0, cfg->m_xTrace->isChecked());
      m_saver->setTraceFlag(1, cfg->m_yTrace->isChecked());
      m_saver->setTraceFlag(2, cfg->m_zTrace->isChecked());
   }
}
void KRotationSetup::lengthEnteredSlot(const QString& s)
{
   m_saver->setTraceLengthSeconds(s.toDouble());
}
void KRotationSetup::LzEnteredSlot(const QString& s)
{
   m_saver->setLz(s.toDouble());
   m_saver->initData();
}
void KRotationSetup::thetaEnteredSlot(const QString& s)
{
   m_saver->setInitEulerTheta(s.toDouble());
   m_saver->initData();
}
