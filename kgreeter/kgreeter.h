/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KGREETER_H
#define KGREETER_H

#include <KStyle>
#include <KGlobalSettings>
#include <Plasma/Theme>
#include <KStandardDirs>

static QFont KGreeterDefaultFont()
{
    return KGlobalSettings::generalFont();
}

static QString KGreeterDefaultStyle()
{
    return QString::fromLatin1("Cleanlooks");
}

static QString KGreeterDefaultCursorTheme()
{
    return QString::fromLatin1(KDE_DEFAULT_CURSOR_THEME);
}

static QString KGreeterDefaultBackground()
{
    return Plasma::Theme::defaultTheme()->wallpaperPath();
}

static QString KGreeterDefaultRectangle()
{
    return KStandardDirs::locate("data", "kgreeter/rectangle.png");
}

#endif // KGREETER_H
