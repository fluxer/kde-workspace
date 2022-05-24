/*
  This file is part of the KDE project

  Copyright (c) 2007 Andreas Hartmetz <ahartmetz@gmail.com>
  Copyright (c) 2007 Michael Jansen <kde@michael-jansen.biz>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
  */

#include "kglobalacceld.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>

static bool isEnabled()
{
    // TODO: Check if kglobalaccel can be disabled
    return true;
}

int main(int argc, char **argv)
{
    // Disable Session Management the right way (C)
    //
    // ksmserver has global shortcuts. disableSessionManagement() does not prevent Qt from
    // registering the app with the session manager. We remove the address to make sure we do not
    // get a hang on kglobalaccel restart (kglobalaccel tries to register with ksmserver,
    // ksmserver tries to register with kglobalaccel).
    unsetenv( "SESSION_MANAGER" );

    KAboutData aboutdata(
            "kglobalaccel",
            0,
            ki18n("KDE Global Shortcuts Service"),
            "0.2",
            ki18n("KDE Global Shortcuts Service"),
            KAboutData::License_LGPL,
            ki18n("(C) 2007-2009  Andreas Hartmetz, Michael Jansen"));
    aboutdata.addAuthor(ki18n("Andreas Hartmetz"),ki18n("Maintainer"),"ahartmetz@gmail.com");
    aboutdata.addAuthor(ki18n("Michael Jansen"),ki18n("Maintainer"),"kde@michael-jansen.biz");

    aboutdata.setProgramIconName("kglobalaccel");

    KCmdLineArgs::init( argc, argv, &aboutdata );

    // check if kglobalaccel is disabled
    if (!isEnabled()) {
        kDebug() << "kglobalaccel is disabled!";
        return 0;
    }

    KApplication app;
    // This app is started automatically, no need for session management
    app.disableSessionManagement();
    app.setQuitOnLastWindowClosed(false);

    QDBusConnection session = QDBusConnection::sessionBus();
    if (!session.isConnected()) {
        kWarning() << "No DBUS session-bus found. Check if you have started the DBUS server.";
        return 1;
    }
    QDBusReply<bool> sessionReply = session.interface()->isServiceRegistered("org.kde.kglobalaccel");
    if (sessionReply.isValid() && sessionReply.value() == true) {
        kWarning() << "Another instance of kglobalaccel is already running!";
        return 2;
    }

    KGlobalAccelD globalaccel;
    if (!globalaccel.init()) {
        return 3;
    }

    return app.exec();
}
