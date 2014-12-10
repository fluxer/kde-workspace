/*============================================================================
 *
 * Screensaver preview area helper class
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
 *===========================================================================*/

#ifndef SSPREVIEWAREA_H
#define SSPREVIEWAREA_H

#include <qwidget.h>

/** @brief Reimplementation of QWidget emitting a signal if resized.
 *
 * This class is equalt to QWidget except for the fact that the signal resized()
 * is emitted if the widget gets resized.  By this signaling mechanism it is
 * possible to resize the embedded GL area object within the screen saver setup
 * dialog.
 *
 * In the constructor of the dialog widget (KPendulumSetup::KPendulumSetup(),
 * KRotationSetup::KRotationSetup()) the signal SsPreviewArea::resized() is
 * connected with a slot of the screensaver class
 * (KPendulumSaver::resizeGlArea(), KRotationSaver::resizeGlArea()).  This slot
 * function calls the reimplemented QGLWidget::resizeGL() method of the GL
 * widgets (PendulumGLWidget::resizeGL(), RotationGLWidget::resizeGL()) which
 * really resizes the GL scenery. */
class SsPreviewArea : public QWidget
{
   Q_OBJECT

  public:
   /** @brief Constructor for SsPreviewArea
    * @param parent Pointer tp parent widget, forwarded to the QWidget
    * constructor
    *
    * The constructor just calls QWidget::QWidget() with the given arguments.
    */
   SsPreviewArea(QWidget* parent = NULL);

  protected:
   /** @brief Called if widget gets resized.
    * @param e Pointer to the corresponding QResizeEvent object containing the
    * resize information
    *
    * Reimplemented event handler from QWidget.  Only the signal resized() is
    * emitted. */
   virtual void resizeEvent(QResizeEvent* e);

  signals:
   /** @brief Signal which is emitted in the resizeEvent() method.
    * @param e Pointer to the corresponding QResizeEvent object */
   void resized(QResizeEvent* e);
};

#endif
