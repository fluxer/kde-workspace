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

#include "sspreviewarea.h"
#include "sspreviewarea.moc"

SsPreviewArea::SsPreviewArea(QWidget* parent)
   : QWidget(parent)
{
}

void SsPreviewArea::resizeEvent(QResizeEvent* e)
{
   emit resized(e);
}
