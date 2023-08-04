/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 *
 * This is free software; you can use this library under the GNU Library
 * General Public License, version 2. See the file "COPYING.LIB" for the
 * exact licensing terms.
 *
 * kcookie.cpp: KDE authentication cookies.
 */

#include "kcookie.h"

#include <stdlib.h>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QProcess>

#include <kdebug.h>

namespace KDESu
{
    namespace KDESuPrivate
    {

        KCookie::KCookie()
        {
#ifdef Q_WS_X11
            m_Display = qgetenv("DISPLAY");
            if (m_Display.isEmpty()) {
                kError() << "$DISPLAY is not set.\n";
                return;
            }
            QByteArray disp = m_Display;
            if (disp.startsWith("localhost:")) {
                disp.remove(0, 9);
            }

            QProcess proc;
            proc.start("xauth", QStringList() << "list" << disp);
            if (!proc.waitForStarted()) {
                kError() << "Could not run xauth.\n";
                return;
            }
            proc.waitForReadyRead(100);
            QByteArray output = proc.readLine().simplified();
            if (output.isEmpty()) {
                kWarning() << "No X authentication info set for display " << m_Display;
                return;
            }
            QList<QByteArray> lst = output.split(' ');
            if (lst.count() != 3) {
                kError() << "parse error.\n";
                return;
            }
            m_DisplayAuth = (lst[1] + ' ' + lst[2]);
            proc.waitForFinished(100); // give QProcess a chance to clean up gracefully
#endif
        }

        QByteArray KCookie::display() const
        {
            return m_Display;
        }

#ifdef Q_WS_X11
        QByteArray KCookie::displayAuth() const
        {
            return m_DisplayAuth;
        }
#endif
    }
}
