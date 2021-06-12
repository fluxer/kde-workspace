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

#include <kdebug.h>

#include "xf86vmodegamma.h"

#include <config-X11.h>

#include <string.h>

// for reference:
// https://cgit.freedesktop.org/xorg/app/xgamma/plain/xgamma.c

// normal gamma for X11 is 1.0, 50 is normal for this class since the UI for configuring screen
// gamma uses slider that has min-max range of 1-100
static const float xgammaratio = 50.0;

XF86VModeGamma::XF86VModeGamma()
    : m_supported(false)
{
    int majorversion, minorversion;
    if (!XF86VidModeQueryVersion(m_x11info.display(), &majorversion, &minorversion)) {
        kWarning() << "XF86VidMode version query failed";
        return;
    }

    int eventbase, errorbase;
    if (!XF86VidModeQueryExtension(m_x11info.display(), &eventbase, &errorbase)) {
        kWarning() << "XF86VidMode extension missing";
        return;
    }
    Q_UNUSED(eventbase);
    Q_UNUSED(errorbase);

    // almost the same version requirement as xgamma application
    if (majorversion < 2) {
        kWarning() << "XF86VidMode version too old" << majorversion << minorversion;
        return;
    }

    m_supported = true;
}

XF86VModeGamma::~XF86VModeGamma()
{
}

bool XF86VModeGamma::isSupported() const
{
    return m_supported;
}

float XF86VModeGamma::gamma() const
{
    float result = 0;

    if (!m_supported) {
        return result;
    }

    XF86VidModeGamma xgamma;
    if (!XF86VidModeGetGamma(m_x11info.display(), m_x11info.screen(), &xgamma)) {
        kWarning() << "Could not query gamma via XF86VidMode";
        return result;
    }
    // result should be in the [0-100] range
    result = (xgammaratio * xgamma.red * xgamma.green * xgamma.blue);

    return result;
}

void XF86VModeGamma::setGamma(float gamma)
{
    if (!m_supported) {
        return;
    }

    XF86VidModeGamma xgamma;
    // in case new member is added
    ::memset(&xgamma, 0, sizeof(XF86VidModeGamma));
    // values should be in the [0.1-10.0] range
    xgamma.red = (gamma / xgammaratio);
    xgamma.green = (gamma / xgammaratio);
    xgamma.blue = (gamma / xgammaratio);
    if (!XF86VidModeSetGamma(m_x11info.display(), m_x11info.screen(), &xgamma)) {
        kWarning() << "Could not set gamma via XF86VidMode";
        return;
    }

    XSync(m_x11info.display(), False);
}
