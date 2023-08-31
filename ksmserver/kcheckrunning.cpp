/* This file is part of the KDE project
   Copyright (C) 2005 Lubos Lunak <l.lunak@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <X11/Xlib.h>

/*
 Return 0 when KDE is running, 1 when KDE is not running but it is possible
 to connect to X, 2 when it's not possible to connect to X.
*/
int main()
{
    Display* dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        return 2;
    }
    // if ksmserver is not started yet check for the X11 atom that startkde sets
    Atom type = None;
    int format = 0;
    unsigned long nitems = 0;
    unsigned long after = 0;
    unsigned char* data = NULL;
    const int kde_full_session_status = XGetWindowProperty(
        dpy, RootWindow(dpy, 0),
        XInternAtom(dpy, "KDE_FULL_SESSION", False),
        0, 0, False, AnyPropertyType, &type, &format, &nitems, &after, &data
    );
    const bool kde_running = (kde_full_session_status == Success && data);
    if (data) {
        XFree(data);
    }
    XCloseDisplay(dpy);
    if (kde_running) {
        return 0;
    }
    return 1;
}
