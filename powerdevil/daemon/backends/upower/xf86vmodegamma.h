/*  This file is part of the KDE project
 *    Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>
 * 
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License version 2 as published by the Free Software Foundation.
 * 
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 * 
 *    You should have received a copy of the GNU Library General Public License
 *    along with this library; see the file COPYING.LIB.  If not, write to
 *    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *    Boston, MA 02110-1301, USA.
 * 
 */

#ifndef XF86VMODEGAMMA_H
#define XF86VMODEGAMMA_H

#include <QtGui/qx11info_x11.h>

#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <fixx11h.h>

class XF86VModeGamma
{
public:
    XF86VModeGamma();
    ~XF86VModeGamma();
    bool isSupported() const;
    float gamma() const;
    void setGamma(float gamma);

private:
    bool m_supported;
    QX11Info m_x11info;
};

#endif // XF86VMODEGAMMA_H
