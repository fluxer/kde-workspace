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

#ifndef ROTATION2_H
#define ROTATION2_H

// Qt headers
#include <qwidget.h>
#include <qtimer.h>
#include <qgl.h>

// GL headers
#include <GL/glu.h>
#include <GL/gl.h>

// KDE headers
#include <kscreensaver.h>
#include <kdialog.h>

// Eigen2 from KDE support
#include <Eigen/Core>
// import most common Eigen types
using namespace Eigen;

// own extension of typdefed vector types of Eigen2
typedef Matrix<double,12,1> Vector12d;

#include "rkodesolver.h"

// KRotationSetupUi
#include "ui_rotationcfg.h"

//--------------------------------------------------------------------

/** @brief ODE solver for the Euler equations.
 *
 * Class implements RkOdeSolver<double> to solve the Euler equations of motion
 * tor the rotating object. */
class EulerOdeSolver : public RkOdeSolver<double,12>
{
  public:
   /** @brief Constructor for the ODE solver for the Euler equations.
    * @param t Time in seconds, integration variable
    * @param dt Initial time increment in seconds for integration, auto adjusted
    * later to guarantee precision
    * @param A Moment of inertia along 1. figure axis
    * @param B Moment of inertia along 2. figure axis
    * @param C Moment of inertia along 3. figure axis
    * @param y Vector of 12 elements containing the initial rotation vector
    * omega (elements 0 to 2), and the initial rotating systems coordinate
    * vectors e1, e2, e3 (elements 3 to 5, 6 to 8, and 9 to 11).
    * @param eps Relative precision per integration step, see
    * RkOdeSolver::RkOdeSolver(). */
   EulerOdeSolver(
      const double& t,
      const double& dt,
      const double& A,
      const double& B,
      const double& C,
      Vector12d&    y,
      const double& eps);

  protected:
   /** @brief ODE function for the Euler equation system
    * @param x time in seconds
    * @param y Vector of 12 elements containing the rotation vector omega
    * (elements 0 to 2), and the rotating systems coordinate vectors e1, e2, e3
    * (elements 3 to 5, 6 to 8, and 9 to 11).
    * @return derivation dy/dx */
   Vector12d f(const double& x, const Vector12d& y) const;

  private:
   /** Moments of inertia along the three figure axes */
   double m_A, m_B, m_C;
};

//--------------------------------------------------------------------

/* forward declaration */
class KRotationSaver;

//--------------------------------------------------------------------

/** @brief GL widget class for the KRotation screen saver
 *
 * Class implements QGLWidget to display the KRotation screen saver. */
class RotationGLWidget : public QGLWidget
{
   Q_OBJECT

  public:
   /** @brief Constructor of KRotation's GL widget
    * @param parent parent widget, passed to QGLWidget's constructor
    * @param omega current rotation vector
    * @param e trace data
    * @param J 3 vector with momenta of inertia with respect to the 3 figure
    * axes. */
   RotationGLWidget(
      QWidget*              parent,
      const KRotationSaver& saver);

  protected:
   /** Called if scenery (GL view) must be updated */
   virtual void paintGL();
   /** Called if gl widget was resized.  Method makes adjustments for new
    * perspective */
   virtual void resizeGL(int w, int h);
   /** Setup the GL environment */
   virtual void initializeGL();

  private:
   /** @brief Draw 3D arrow
    * @param total_length total length of arrow
    * @param head_length length of arrow head (cone)
    * @param base_width width of arrow base
    * @param head_width width of arrow head (cone)
    *
    * The arrow is drawn from the coordinates zero point along th z direction.
    * The cone's tip is located at (0,0,@a total_length). */
   void myGlArrow(
      GLfloat total_length,
      GLfloat head_length,
      GLfloat base_width,
      GLfloat head_width);
   /** Draw the traces in the GL area */
   void draw_traces (void);

  private: // Private attributes
   /** Eye position distance from coordinate zero point */
   GLfloat  m_eyeR;
   /** Eye position theta angle from z axis */
   GLfloat  m_eyeTheta;
   /** Eye position phi angle (longitude) */
   GLfloat  m_eyePhi;
   /** Box size */
   Vector3d m_boxSize;
   /** GL object list of fixed coordinate systems axses */
   GLuint   m_fixedAxses;
   /** GL object list of rotating coordinate systems axses */
   GLuint   m_bodyAxses;
   /** Light position distance from coordinate zero point */
   GLfloat  m_lightR;
   /** Light position theta angle from z axis */
   GLfloat  m_lightTheta;
   /** Light position phi angle (longitude) */
   GLfloat  m_lightPhi;

   /** Length of the rotating coordinate system axses */
   GLfloat  m_bodyAxsesLength;
   /** Length of the fixed coordinate system axses */
   GLfloat  m_fixedAxsesLength;

   /** reference to screen saver */
   const KRotationSaver& m_saver;
};

//--------------------------------------------------------------------

/** @brief Main class of the KRotation screen saver
 *
 * This class implements KScreenSaver for the KRotation screen saver. */
class KRotationSaver : public KScreenSaver
{
   Q_OBJECT

  public:

   /* public member functions */

   /** @brief Constructor of the KRotation screen saver object
    * @param drawable Id of the window in which the screen saver is drawed
    *
    * Initial settings are read from disk, the GL widget is set up and displayed
    * and the eq. of motion solver is started. */
   KRotationSaver(WId drawable);
   /** @brief Destructor of the KPendulum screen saver object
    *
    * Only KPendulumSaver::solver is destoyed. */
   ~KRotationSaver();
   /** read the saved settings from disk */
   void readSettings();
   /** init physical quantities and set up the ode solver */
   void initData();

   /** Returns length of traces in seconds of visibility, parameter from setup
    * dialog */
   inline double traceLengthSeconds(void) const
      {
         return m_traceLengthSeconds;
      }
   /** Sets the length of traces in seconds of visibility. */
   void setTraceLengthSeconds(const double& t);

   /** Flags indicating if the traces for x,y,z are shown.  Only relevant if
    * ::randomTraces is not set to true.  Parameter from setup dialog */
   inline bool traceFlag(unsigned int n) const
      {
         return m_traceFlag[n];
      }
   /** (Un)Sets the x,y,z traces flags. */
   inline void setTraceFlag(unsigned int n, const bool& flag)
      {
         m_traceFlag[n] = flag;
      }

   /** If flag is set to true the traces will be (de)activated randomly all 10
    * seconds.  Parameter from setup dialog */
   inline bool randomTraces(void) const
      {
         return m_randomTraces;
      }
   /** (Un)Sets the random trace flag. */
   inline void setRandomTraces(const bool& flag)
      {
         m_randomTraces = flag;
      }

   /** Returns the angular momentum. */
   inline double Lz(void) const
      {
         return m_Lz;
      }
   /** Sets the angular momentum. */
   void setLz(const double& Lz);

   /** Returns initial eulerian angle theta of the top body at t=0 sec. */
   inline double initEulerTheta(void) const
      {
         return m_initEulerTheta;
      }
   /** Set the initial eulerian angle theta of the top body at t=0 sec. */
   void setInitEulerTheta(const double& theta);

   /** Returns constant reference to m_omega */
   inline const Vector3d& omega(void) const
      {
         return m_omega;
      }
   /** Returns constant reference to m_e */
   inline const std::deque<Matrix3d>& e(void) const
      {
         return m_e;
      }
   /** Returns constant reference to m_J */
   inline const Vector3d& J(void) const
      {
         return m_J;
      }

   /* public static class member variables */

   /** Lower argument limit for setTraceLengthSeconds() */
   static const double sm_traceLengthSecondsLimitLower;
   /** Upper argument limit for setTraceLengthSeconds() */
   static const double sm_traceLengthSecondsLimitUpper;
   /** Default value of KRotationSaver::m_traceLengthSeconds */
   static const double sm_traceLengthSecondsDefault;

   /** Default values for KRotationSaver::m_traceFlag */
   static const bool sm_traceFlagDefault[3];
   /** Default value for KRotationSaver::m_randomTraces */
   static const bool sm_randomTracesDefault;

   /** Lower argument limit for setLz() */
   static const double sm_LzLimitLower;
   /** Upper argument limit for setLz() */
   static const double sm_LzLimitUpper;
   /** Default value for KRotationSaver::m_Lz */
   static const double sm_LzDefault;

   /** Lower argument limit for setInitEulerTheta() */
   static const double sm_initEulerThetaLimitLower;
   /** Upper argument limit for setInitEulerTheta() */
   static const double sm_initEulerThetaLimitUpper;
   /** Default value for KRotationSaver::m_initEulerTheta */
   static const double sm_initEulerThetaDefault;

  public slots:
   /** slot is called if integration should proceed by ::sm_deltaT */
   void doTimeStep();
   /** slot is called if setup dialog changes in size and the GL area should be
    * adjusted */
   void resizeGlArea(QResizeEvent* e);

  private:

   /** Time step size for the integration in milliseconds.  Used in
    * ::KRotationSaver and ::RotationGLWidget. */
   static const unsigned int sm_deltaT;

   /** The ode solver which is used to integrate the equations of motion */
   EulerOdeSolver*   m_solver;
   /** Gl widget of simulation */
   RotationGLWidget* m_glArea;
   /** Timer for the real time integration of the Euler equations */
   QTimer*           m_timer;

   /** current rotation vector */
   Vector3d             m_omega;
   /** deque of matrices of figure axes in fixed frame coordinates.  Each matrix
    * column represents an axis vector */
   std::deque<Matrix3d> m_e;
   /** Momentum of inertia along figure axes */
   const Vector3d       m_J;

   /** Length of traces in seconds of visibility, parameter from setup dialog */
   double   m_traceLengthSeconds;
   /** Flags indicating if the traces for x,y,z are shown.  Only relevant if
    * ::randomTraces is not set to true.  Parameter from setup dialog */
   bool     m_traceFlag[3];
   /** If flag is set to true the traces will be (de)activated randomly all 10
    * seconds.  Parameter from setup dialog */
   bool     m_randomTraces;
   /** Angular momentum.  This is a constant of motion and points always into
    * positive z direction.  Parameter from setup dialog */
   double   m_Lz;

   /** Initial eulerian angles phi of the top body at t=0s */
   double   m_initEulerPhi;
   /** Initial eulerian angles psi of the top body at t=0s */
   double   m_initEulerPsi;
   /** Initial eulerian angles theta of the top body at t=0 sec.  Parameter from
    * setup dialog */
   double   m_initEulerTheta;
};

class RotationWidget : public QWidget, public Ui::RotationWidget
{
public:
    RotationWidget( QWidget *parent = 0L ) : QWidget( parent ) {
        setupUi( this );
    }
};

//--------------------------------------------------------------------

/** @brief KRotation screen saver setup dialog.
 *
 * This class handles the KRotation screen saver setup dialog. */
class KRotationSetup : public KDialog
{
   Q_OBJECT

  public:
   KRotationSetup(QWidget* parent = NULL);
   ~KRotationSetup();

  public slots:
   /// slot for the OK Button: save settings and exit
   void okButtonClickedSlot(void);
   /// slot for the About Button: show the About dialog
   void aboutButtonClickedSlot(void);
   void randomTracesToggled(bool state);
   void xTraceToggled(bool state);
   void yTraceToggled(bool state);
   void zTraceToggled(bool state);
   void lengthEnteredSlot(const QString& s);
   void LzEnteredSlot(const QString& s);
   void thetaEnteredSlot(const QString& s);

  private:
   /// the screen saver widget which is displayed in the preview area
   KRotationSaver* m_saver;
    RotationWidget *cfg;
};

#endif
