/*============================================================================
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
 *
 *============================================================================*/

#ifndef PENDULUM_H
#define PENDULUM_H

// STL headers
#include <valarray>

// Qt headers
#include <QWidget>
#include <QTimer>
#include <QGLWidget>

// GL headers
#include <GL/glu.h>
#include <GL/gl.h>

// KDE headers
#include <kscreensaver.h>

#include "rkodesolver.h"

// KPendulumSetupUi
#include "ui_pendulumcfg.h"

//--------------------------------------------------------------------

/** @brief ODE solver for the Pendulum equations */
class PendulumOdeSolver : public RkOdeSolver<double,4>
{
  public:
   /** @brief Constuctor for the RK solver of the pendulum equation of motion
    * @param t initial time in seconds
    * @param dt initial time increment in seconds, just a hint for solver
    * @param y generalized coordinates of pendulum system
    * @param eps relative precision
    * @param m1 mass of upper pendulum
    * @param m2 mass of lower pendulum
    * @param l1 length of upper pendulum
    * @param l2 length of lower pendulum
    * @param g gravitational constant */
   PendulumOdeSolver(
      const double&   t,
      const double&   dt,
      const Vector4d& y,
      const double&   eps,
      const double&   m1,
      const double&   m2,
      const double&   l1,
      const double&   l2,
      const double&   g);

  protected:
   /** @brief ODE function for the pendulum equation of motion system
    * @param x time
    * @param y generalized coordinates of pendulum system
    * @return derivation dy/dx */
   Vector4d f(const double& x, const Vector4d& y) const;

  private:
   /** These private variables contain constants for faster numeric calculation.
    * They are derived from the constructor arguments m1,m2,l1,l2,g.  */
   const double m_A, m_B1, m_B, m_C, m_D, m_E, m_M;
};


//--------------------------------------------------------------------

/** @brief GL widget class for the KPendulum screen saver
 *
 * Class implements QGLWidget to display the KPendulum screen saver. */
class PendulumGLWidget : public QGLWidget
{
   Q_OBJECT

  public:
   /** @brief Constructor of KPendulum's GL widget
    * @param parent parent widget, passed to QGLWidget's constructor */
   PendulumGLWidget(QWidget* parent=0);
   /** @brief Destructor of KPendulum's GL widget */
   ~PendulumGLWidget(void);

   /** @brief Set phi angle of viewpoint
    * @param phi angle in sterad */
   void setEyePhi(double phi);
   /** @brief Set angles of pendulum configuration
    * @param q1 angle of 1. pendulum in sterad
    * @param q2 angle of 2. pendulum in sterad */
   void setAngles(const double& q1, const double& q2);
   /** @brief Set masses of pendulum configuration
    * @param m1 mass of 1. pendulum
    * @param m2 mass of 2. pendulum */
   void setMasses(const double& m1, const double& m2);
   /** @brief Set lengths of pendulum configuration
    * @param l1 length of 1. pendulum
    * @param l2 length of 2. pendulum */
   void setLengths(const double& l1, const double& l2);

   /* accessors for colour settings */

   /** @brief set color of the bars
    * @param c color */
   void setBarColor(const QColor& c);
   /** @brief get color of the bars
    * @return color */
   inline QColor barColor(void) const {return m_barColor;}
   /** @brief set color of mass 1
    * @param c color */
   void setM1Color(const QColor& c);
   /** @brief get color of mass 1
    * @return color */
   inline QColor m1Color(void) const {return m_m1Color;}
   /** @brief set color of mass 2
    * @param c color */
   void setM2Color(const QColor& c);
   /** @brief get color of mass 2
    * @return color */
   inline QColor m2Color(void) const {return m_m2Color;}

  protected:
   /** paint the GL view */
   virtual void paintGL();
   /** resize the gl view */
   virtual void resizeGL(int w, int h);
   /** setup the GL environment */
   virtual void initializeGL();

  private: // Private attributes
   /** Eye position distance from coordinate zero point */
   GLfloat m_eyeR;
   /** Eye position theta angle from z axis in sterad */
   double  m_eyeTheta;
   /** Eye position phi angle (longitude) in sterad */
   double  m_eyePhi;
   /** Light position distance from coordinate zero point */
   GLfloat m_lightR;
   /** Light position theta angle from z axis in sterad */
   double  m_lightTheta;
   /** Light position phi angle (longitude) in sterad */
   double  m_lightPhi;

   /** 1. pendulum's angle, degree */
   GLfloat m_ang1;
   /** 2. pendulum's angle, degree */
   GLfloat m_ang2;

   /** 1. pendulum's square root of mass */
   GLfloat m_sqrtm1;
   /** 2. pendulum's square root of mass */
   GLfloat m_sqrtm2;

   /** 1. pendulum's length */
   GLfloat m_l1;
   /** 2. pendulum's length */
   GLfloat m_l2;

   /** Pointer to a quadric object used in the rendering function paintGL() */
   GLUquadricObj* const m_quadM1;

   /** color of the pendulum bars */
   QColor m_barColor;
   /** color of the 1. mass */
   QColor m_m1Color;
   /** color of the 2. mass */
   QColor m_m2Color;
};

//--------------------------------------------------------------------

/** @brief Main class of the KPendulum screen saver
 *
 * This class implements KScreenSaver for the KPendulum screen saver. */
class KPendulumSaver : public KScreenSaver
{
   Q_OBJECT

  public:
   /** @brief Constructor of the KPendulum screen saver object
    * @param drawable Id of the window in which the screen saver is drawed
    *
    * Initial settings are read from disk, the GL widget is set up and displayed
    * and the eq. of motion solver is started. */
   KPendulumSaver(WId drawable);
   /** @brief Destructor of the KPendulum screen saver object
    *
    * Only KPendulumSaver::solver is destoyed. */
   ~KPendulumSaver();

   /** read the saved settings from disk */
   void readSettings();
   /** init physical quantities, set up the GL area and (re)start the ode
    * solver.  Called if new parameters are specified in the setup dialog and at
    * startup. */
   void initData();

   /* accessors for PendulumGLWidget member variables */

   /** Set the displayed bar color of the pendulum */
   void setBarColor(const QColor& c);
   /** Get the displayed bar color of the pendulum */
   QColor barColor(void) const;

   /** Set the displayed color of the 1. pendulum mass */
   void setM1Color(const QColor& c);
   /** Get the displayed color of the 1. pendulum mass */
   QColor m1Color(void) const;

   /** Set the displayed color of the 2. pendulum mass */
   void setM2Color(const QColor& c);
   /** Get the displayed color of the 2. pendulum mass */
   QColor m2Color(void) const;

   /* accessors for own member variables */

   /** Set the mass ratio of the pendulum system. @sa
    * KPendulumSaver::m_massRatio */
   void setMassRatio(const double& massRatio);
   /** Get the mass ratio of the pendulum system. @sa
    * KPendulumSaver::m_massRatio */
   inline double massRatio(void) const
      {
         return m_massRatio;
      }

   /** Set the length ratio of the pendulum system. @sa
    * KPendulumSaver::m_lengthRatio */
   void setLengthRatio(const double& lengthRatio);
   /** Get the length ratio of the pendulum system. @sa
    * KPendulumSaver::m_lengthRatio */
   inline double lengthRatio(void) const
      {
         return m_lengthRatio;
      }

   /** Set the gravitational constant. @sa KPendulumSaver::m_g */
   void setG(const double& g);
   /** Get the gravitational constant. @sa KPendulumSaver::m_g */
   inline double g(void) const
      {
         return m_g;
      }

   /** Set the total energy. @sa KPendulumSaver::m_E */
   void setE(const double& E);
   /** Get the total energy. @sa KPendulumSaver::m_E */
   inline double E(void) const
      {
         return m_E;
      }

   /** Set the time interval for the periodic perspective change. @sa
    * KPendulumSaver::m_persChangeInterval */
   void setPersChangeInterval(const unsigned int& persChangeInterval);
   /** Get the time interval for the periodic perspective change. @sa
    * KPendulumSaver::m_persChangeInterval */
   inline unsigned int persChangeInterval(void) const
      {
         return m_persChangeInterval;
      }

   /* public static class member variables */

   static const QColor sm_barColorDefault;
   static const QColor sm_m1ColorDefault;
   static const QColor sm_m2ColorDefault;

   /** lower, upper limits (inclusive) and default values for the setup
    * parameter massRatio */
   static const double sm_massRatioLimitUpper;
   static const double sm_massRatioLimitLower;
   static const double sm_massRatioDefault;

   /** lower, upper limits (inclusive) and default values for the setup
    * parameter lengthRatio */
   static const double sm_lengthRatioLimitLower;
   static const double sm_lengthRatioLimitUpper;
   static const double sm_lengthRatioDefault;

   /** lower, upper limits (inclusive) and default values for the setup
    * parameter g */
   static const double sm_gLimitLower;
   static const double sm_gLimitUpper;
   static const double sm_gDefault;

   /** lower, upper limits (inclusive) and default values for the setup
    * parameter E */
   static const double sm_ELimitLower;
   static const double sm_ELimitUpper;
   static const double sm_EDefault;

   /** lower, upper limits (inclusive) and default values for the setup
    * parameter persChangeInterval */
   static const unsigned int sm_persChangeIntervalLimitLower;
   static const unsigned int sm_persChangeIntervalLimitUpper;
   static const unsigned int sm_persChangeIntervalDefault;

  public slots:
   /** slot is called if integration should proceed by ::deltaT */
   void doTimeStep();
   /** slot is called if setup dialog changes in size and the GL are should be
    * adjusted */
   void resizeGlArea(QResizeEvent* e);

  private:
   /** Time step size for the integration in milliseconds.  20 ms corresponds to
    * a frame rate of 50 fps. */
   static const unsigned int sm_deltaT;
   /** Default angle phi for the eye to look onto the pendulum */
   static const double       sm_eyePhiDefault;

   /** The ode solver which is used to integrate the equations of motion */
   PendulumOdeSolver* m_solver;
   /** Gl widget of simulation */
   PendulumGLWidget*  m_glArea;
   /** Timer for the real time integration of the eqs. of motion */
   QTimer*            m_timer;

   // persistent configurtion settings

   /** Mass ratio m2/(m1+m2) of the pendulum masses.  Value is determined by the
    * setup dialog.  Variable is accessed by setMassRatio() and massRatio(). */
   double       m_massRatio;
   /** Length ratio l2/(l1+l2) of the pendulums.  Value is determined by the
    * setup dialog.  Variable is accessed by setLengthRatio() and
    * lengthRatio(). */
   double       m_lengthRatio;
   /** Gravitational constant (in arbitrary units).  Value is determined by the
    * setup dialog.  Variable is accessed by setG() and g(). */
   double       m_g;
   /** Total energy of the system in units of the maximum possible potential
    * energy.  Value is determined by the setup dialog.  Variable is accessed by
    * setE() and E(). */
   double       m_E;
   /** Time interval after which a new perspective changed happens.  Value is
    * determined by the setup dialog.  Variable is accessed by
    * setPersChangeInterval() and persChangeInterval(). */
   unsigned int m_persChangeInterval;
};

//--------------------------------------------------------------------

class PendulumWidget : public QWidget, public Ui::PendulumWidget
{
public:
    PendulumWidget( QWidget *parent = 0L ) : QWidget( parent ) {
        setupUi( this );
    }
};

/** @brief KPendulum screen saver setup dialog.
 *
 * This class handles the KPendulum screen saver setup dialog. */
class KPendulumSetup : public KDialog
{
   Q_OBJECT

  public:
   /** @brief Constructor for the KPendulum screen saver setup dialog
    * @param parent Pointer to the parent widget, passed to KPendulumSetupUi
    *
    * The dialog box is set up and the screen saver object KPendulumSetup::saver
    * is instantiated. */
   KPendulumSetup(QWidget* parent = 0);
   /** @brief Destructor of the KPendulum screen saver setup dialog
    *
    * Only KPendulumSetup::saver is deleted. */
   ~KPendulumSetup(void);

  public slots:
   /** slot for the "OK" button: save settings and exit */
   void okButtonClickedSlot(void);
   /** slot for the "About" button: show the About dialog */
   void aboutButtonClickedSlot(void);

   /** slot is called if the mass ratio edit field looses its focus.  If the
    * input is acceptable KPendulumSaver::setMassRatio() is called. */
   void mEditLostFocusSlot(void);
   /** slot is called if the length ratio edit field looses its focus.  If the
    * input is acceptable KPendulumSaver::setLengthRatio() is called. */
   void lEditLostFocusSlot(void);
   /** slot is called if the gravitational constant edit field looses its focus.
    * If the input is acceptable KPendulumSaver::setG() is called. */
   void gEditLostFocusSlot(void);
   /** slot is called if the energy edit field looses its focus.  If the input
    * is acceptable KPendulumSaver::setE() is called. */
   void eEditLostFocusSlot(void);
   /** slot is called if the perspective change interval spin box changed.  If
    * the input is acceptable KPendulumSaver::setPersChangeInterval() is
    * called. */
   void persChangeEnteredSlot(int t);

   /** slot is called if the bar color button was clicked.  A color dialog is
    * opened and the result is given to KPendulumSaver::setBarColor(). */
   void barColorButtonClickedSlot(void);
   /** slot is called if the mass 1 color button was clicked.  A color dialog is
    * opened and the result is given to KPendulumSaver::setM1Color(). */
   void m1ColorButtonClickedSlot(void);
   /** slot is called if the mass 2 color button was clicked.  A color dialog is
    * opened and the result is given to KPendulumSaver::setM2Color(). */
   void m2ColorButtonClickedSlot(void);

  private:
   /** Pointer to the screen saver object.  Its member KPendulumSaver::glArea is
    * displayed in the preview area */
   KPendulumSaver* m_saver;
    PendulumWidget *cfg;
};

#endif
