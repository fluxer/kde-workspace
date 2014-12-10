/** @file
 *
 * Ordinary differential equation solver using the Runge-Kutta method.
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

#ifndef RKODESOLVER_H
#define RKODESOLVER_H

#include <KDebug>

/* vector and matrix classes */
#include <Eigen/Core>
/* import most common Eigen types */
using namespace Eigen;


/** @brief Solver class to integrate a first-order ordinary differential
 * equation (ODE) by means of a 6. order Runge-Kutta method.
 *
 * See the article about the Cash-Karp method
 * (http://en.wikipedia.org/wiki/Cash%E2%80%93Karp_method) for details on this
 * algorithm.
 *
 * The ODE system must be given as the derivative
 * dy/dx = f(x,y)
 * with x in R and y in R^n.
 *
 * Within this class the function f() is a pure virtual function, which must be
 * reimplemented in a derived class. */
template<typename T, int D>
class RkOdeSolver
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

   /** @brief Constructor
    * @param x Initial integration parameter
    * @param y Initial function values of function to integrate
    * @param dx Initial guess for step size.  Will be automatically adjusted to
    * guarantee required precision. @a dx > 0
    * @param eps Relative precision. @a eps > 0.
    *
    * Initialises the solver with start conditions. */
   RkOdeSolver(
      const T&             x,//   = 0.0,
      const Matrix<T,D,1>& y,//   = Matrix<T,D,1>::Zero(),
      const T&             dx  = 0.0,
      const T&             eps = 1e-6);

   /** @brief Destructor */
   virtual ~RkOdeSolver(void);

   /** @brief Integrates the ordinary differential equation from the current x
    * value to x+@a dx.
    * @param dx x-interval size to integrate over starting from x.  dx may be
    * negative.
    *
    * The integration is performed by calling rkStepCheck() repeatedly until the
    * desired x value is reached. */
   void integrate(const T& dx);

   // Accessors

   /** @brief Get current x value.
    * @return Reference of x value. */
   const T& X(void) const;
   /** @brief Set current x value.
    * @param a The value to be set. */
   void X(const T& a);

   /** @brief Get current y value.
    * @return Reference of y vector. */
   const Matrix<T,D,1>& Y(void) const;
   /** @brief Set current y values.
    * @param a The vector to be set. */
   void Y(const Matrix<T,D,1>& a);

   /** @brief Get current dy/dx value.
    * @return Reference of dy/dx vector. */
   const Matrix<T,D,1>& dYdX(void) const;

   /** @brief Get current estimated step size dX.
    * @return Reference of dX value. */
   const T& dX(void) const;
   /** @brief Set estimated step size dX.
    * @param a The value to be set. */
   void dX(const T& a);

   /** @brief Get current presision.
    * @return Reference of precision value. */
   const T& Eps(void) const;
   /** @brief Set estimated presision.
    * @param a The value to be set. */
   void Eps(const T& a);

  protected:
   /** @brief ODE function
    * @param x Integration value
    * @param y Function value
    * @return Derivation
    *
    * This purely virtual function returns the value of dy/dx for the given
    * parameter values of x and y. This function is integrated by the
    * Runge-Kutta algorithm. */
   virtual Matrix<T,D,1>
      f(const T& x, const Matrix<T,D,1>& y) const = 0;

  private:
   /** @brief Perform one integration step with a tolerable relative error given
    * by ::mErr.
    * @param dx Maximal step size, may be positive or negative depending on
    * integration direction.
    * @return Flag indicating if made absolute integration step was equal |@a dx
    * | (true) less than |@a dx | (false).
    *
    * A new estimate for the maximum next step size is saved to ::m_step.  The
    * new values for x, y and f are saved in ::m_x, ::m_y and ::m_dydx. */
   bool rkStepCheck(const T& dx);

   /** @brief Perform one Runge-Kutta integration step forward with step size
    * ::m_step
    * @param dx Step size relative to current x value.
    * @param yerr Reference to vector in which the estimated error made in y is
    * returned.
    * @return The y value after the step at x+@a dx.
    *
    * Stored current x,y values are not adjusted. */
   Matrix<T,D,1> rkStep(
      const T& dx, Matrix<T,D,1>& yerr) const;

   /** current x value */
   T             m_x;
   /** current y value */
   Matrix<T,D,1> m_y;
   /** current value of dy/dx */
   Matrix<T,D,1> m_dydx;

   /** allowed relative error */
   T             m_eps;
   /** estimated step size for next Runge-Kutta step */
   T             m_step;
};

// inline accessors

template<typename T, int D>
inline const T&
RkOdeSolver<T, D>::X(void) const
{
   return m_x;
}

template<typename T, int D>
inline void
RkOdeSolver<T, D>::X(const T &a)
{
   m_x = a;
}

template<typename T, int D>
inline const Matrix<T,D,1>&
RkOdeSolver<T, D>::Y(void) const
{
   return m_y;
}

template<typename T, int D>
inline void
RkOdeSolver<T, D>::Y(const Matrix<T,D,1>& a)
{
   m_y = a;
}

template<typename T, int D>
inline const Matrix<T,D,1>&
RkOdeSolver<T, D>::dYdX(void) const
{
   return m_dydx;
}

template<typename T, int D>
inline const T&
RkOdeSolver<T, D>::dX(void) const
{
   return m_step;
}

template<typename T, int D>
inline const T&
RkOdeSolver<T, D>::Eps(void) const
{
   return m_eps;
}


template<typename T, int D>
RkOdeSolver<T, D>::RkOdeSolver(
   const T&             x,
   const Matrix<T,D,1>& y,
   const T&             dx,
   const T&             eps)
   : m_x(x)
{
   Y(y);
   dX(dx);
   Eps(eps);
}

// virtual dtor
template<typename T, int D>
RkOdeSolver<T, D>::~RkOdeSolver(void)
{
}

// accessors

template<typename T, int D>
void
RkOdeSolver<T, D>::dX(const T& a)
{
   if (a <= 0.0)
   {
      kError() << "RkOdeSolver: dx must be > 0";
      m_step = 0.001;            // a very arbitrary value
      return;
   }

   m_step = a;
}

template<typename T, int D>
void
RkOdeSolver<T, D>::Eps(const T& a)
{
   if (a <= 0.0)
   {
      kError() << "RkOdeSolver: eps must be > 0";
      m_eps = 1e-5;              // a very arbitrary value
      return;
   }

   m_eps = a;
}

// public member functions

template<typename T, int D>
void
RkOdeSolver<T, D>::integrate(const T& deltaX)
{
   if (deltaX == 0)
   {
      return;                   // nothing to integrate
   }

   // init dydx
   m_dydx = f(m_x, m_y);

   static const unsigned int maxiter = 10000;
   const T x2 = m_x + deltaX;

   unsigned int iter;
   for (iter=0;
        iter<maxiter && !rkStepCheck(x2-m_x);
        ++iter)
   {
   }

   if (iter > maxiter)
   {
      kWarning()
         << "RkOdeSolver: More than " << maxiter
         << " iterations in RkOdeSolver::integrate" << endl;
   }
}


// private member functions

template<typename T, int D>
bool
RkOdeSolver<T, D>::rkStepCheck(const T& dx_requested)
{
   static const T safety =  0.9;
   static const T pshrnk = -0.25;
   static const T pgrow  = -0.2;

   // reduce step size by no more than a factor 10
   static const T shrinkLimit = 0.1;
   // enlarge step size by no more than a factor 5
   static const T growthLimit = 5.0;
   // errmax_sl = 6561.0
   static const T errmax_sl = std::pow(shrinkLimit/safety, 1.0/pshrnk);
   // errmax_gl = 1.89e-4
   static const T errmax_gl = std::pow(growthLimit/safety, 1.0/pgrow);

   static const unsigned int maxiter = 100;

   if (dx_requested == 0)
   {
      return true;              // integration done
   }

   Matrix<T,D,1> ytmp, yerr, t;

   bool stepSizeWasMaximal;
   T dx;
   if (std::abs(dx_requested) > m_step)
   {
      stepSizeWasMaximal = true;
      dx = dx_requested>0 ? m_step : -m_step;
   }
   else
   {
      stepSizeWasMaximal = false;
      dx = dx_requested;
   }

   // generic scaling factor
   // |y| + |dx * dy/dx| + 1e-15
   Matrix<T,D,1> yscal
      = (m_y.cwise().abs() + (dx*m_dydx).cwise().abs()).cwise()
      + 1e-15;

   unsigned int iter = 0;
   T errmax = 0;
   do
   {
      if (errmax >= 1.0)
      {
         // reduce step size
         dx *= errmax<errmax_sl ? safety * pow(errmax, pshrnk) : shrinkLimit;
         stepSizeWasMaximal = true;
         if (m_x == m_x + dx)
         {
            // stepsize below numerical resolution
            kError()
               << "RkOdeSolver: stepsize underflow in rkStepCheck"
               << endl;
         }
         // new dx -> update scaling vector
         yscal
            = (m_y.cwise().abs()
               + (dx*m_dydx).cwise().abs()).cwise()
            + 1e-15;
      }

      ytmp   = rkStep(dx, yerr); // try to make a step forward
      t      = (yerr.cwise() / yscal).cwise().abs(); // calc the error vector
      errmax = t.maxCoeff()/m_eps;    // calc the rel. maximal error
      ++iter;
   } while ((iter < maxiter) && (errmax >= 1.0));

   if (iter >= maxiter)
   {
      kError()
         << "RkOdeSolver: too many iterations in rkStepCheck()";
   }

   if (stepSizeWasMaximal)
   {
      // estimate next step size if used step size was maximal
      m_step =
         std::abs(dx)
         * (errmax>errmax_gl ? safety * pow(errmax, pgrow) : growthLimit);
   }
   m_x    += dx;                // make step forward
   m_y     = ytmp;              // save new function values
   m_dydx  = f(m_x,m_y);        // and update derivatives

   return (std::abs(dx) < std::abs(dx_requested));
}

template<typename T, int D>
Matrix<T,D,1>
RkOdeSolver<T, D>::rkStep(const T& dx, Matrix<T,D,1>& yerr) const
{
   static const T
      a2=0.2, a3=0.3, a4=0.6, a5=1.0, a6=0.875,
      b21=0.2,
      b31=3.0/40.0,       b32=9.0/40.0,
      b41=0.3,            b42=-0.9,        b43=1.2,
      b51=-11.0/54.0,     b52=2.5,         b53=-70.0/27.0, b54=35.0/27.0,
      b61=1631.0/55296.0, b62=175.0/512.0, b63=575.0/13824.0,
      b64=44275.0/110592.0, b65=253.0/4096.0,
      c1=37.0/378.0, c3=250.0/621.0, c4=125.0/594.0, c6=512.0/1771.0,
      dc1=c1-2825.0/27648.0,  dc3=c3-18575.0/48384.0,
      dc4=c4-13525.0/55296.0, dc5=-277.0/14336.0, dc6=c6-0.25;

   Matrix<T,D,1> ak2 = f(m_x + a2*dx,
                         m_y + dx*b21*m_dydx);             // 2. step
   Matrix<T,D,1> ak3 = f(m_x + a3*dx,
                         m_y + dx*(b31*m_dydx + b32*ak2)); // 3.step
   Matrix<T,D,1> ak4 = f(m_x + a4*dx,
                         m_y + dx*(b41*m_dydx + b42*ak2
                                   + b43*ak3));           // 4.step
   Matrix<T,D,1> ak5 = f(m_x + a5*dx,
                         m_y + dx*(b51*m_dydx + b52*ak2
                                   + b53*ak3 + b54*ak4)); // 5.step
   Matrix<T,D,1> ak6 = f(m_x + a6*dx,
                         m_y + dx*(b61*m_dydx + b62*ak2
                                   + b63*ak3 + b64*ak4
                                   + b65*ak5));           // 6.step
   yerr       = dx*(dc1*m_dydx + dc3*ak3 + dc4*ak4 + dc5*ak5 + dc6*ak6);
   return m_y + dx*( c1*m_dydx +            c3*ak3 +  c4*ak4 +  c6*ak6);
}

#endif
