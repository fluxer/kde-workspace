/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2007 Rivo Laks <rivolaks@hot.ee>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "config-kwin.h"

#include "compositingprefs.h"

#include "xcbutils.h"

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kxerrorhandler.h>
#include <KGlobal>
#include <KLocalizedString>
#include <kdeversion.h>
#include <ksharedconfig.h>
#include <kstandarddirs.h>

#include <qprocess.h>


namespace KWin
{

extern int screen_number; // main.cpp
extern bool is_multihead;

CompositingPrefs::CompositingPrefs()
{
}

CompositingPrefs::~CompositingPrefs()
{
}

bool CompositingPrefs::compositingPossible()
{
    if (!Xcb::Extensions::self()->isCompositeAvailable()) {
        kDebug(1212) << "No composite extension available";
        return false;
    }
    if (!Xcb::Extensions::self()->isDamageAvailable()) {
        kDebug(1212) << "No damage extension available";
        return false;
    }
#ifdef KWIN_BUILD_COMPOSITE
    if (Xcb::Extensions::self()->isRenderAvailable() && Xcb::Extensions::self()->isFixesAvailable())
        return true;
#endif
    kDebug(1212) << "No XRender/XFixes support";
    return false;
}

QString CompositingPrefs::compositingNotPossibleReason()
{
    if (!Xcb::Extensions::self()->isCompositeAvailable() || !Xcb::Extensions::self()->isDamageAvailable()) {
        return i18n("Required X extensions (XComposite and XDamage) are not available.");
    }
#if defined( KWIN_BUILD_COMPOSITE )
    if (!Xcb::Extensions::self()->isRenderAvailable() || !Xcb::Extensions::self()->isFixesAvailable()) {
        return i18n("XRender/XFixes are not available.");
    }
#endif
    return QString();
}

} // namespace

