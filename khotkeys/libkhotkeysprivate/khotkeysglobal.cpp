/****************************************************************************

 KHotKeys
 
 Copyright (C) 1999-2001 Lubos Lunak <l.lunak@kde.org>

 Distributed under the terms of the GNU General Public License version 2.
 
****************************************************************************/

#define _KHOTKEYSGLOBAL_CPP_

#include "khotkeysglobal.h"

#include <kdebug.h>
#include <kstandarddirs.h>

#include "input.h"
#include "windows_handler.h"
#include "triggers/triggers.h"
#include "shortcuts_handler.h"

namespace KHotKeys
{

QPointer<ShortcutsHandler> keyboard_handler = NULL;
QPointer<WindowsHandler> windows_handler = NULL;

static bool _khotkeys_active = false;

void init_global_data( bool active_P, QObject* owner_P )
    {
    // FIXME: get rid of that static_cast<>s. Don't know why they are there.
    // Make these singletons.
    if (!keyboard_handler)
        {
        keyboard_handler = new ShortcutsHandler( active_P ? ShortcutsHandler::Active : ShortcutsHandler::Configuration, owner_P );
        }
    if (!windows_handler)
        {
        windows_handler = new WindowsHandler( active_P, owner_P );
        }
    khotkeys_set_active( false );
    }
    
void khotkeys_set_active( bool active_P )
    {
    _khotkeys_active = active_P;
    }
    
bool khotkeys_active()
    {
    return _khotkeys_active;
    }

} // namespace KHotKeys
