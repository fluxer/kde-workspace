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

#include "kgreeterhelper.h"

#include <QSettings>
#include <kdebug.h>

#include "config-workspace.h"

int KGreeterHelper::save(const QVariantMap &parameters)
{
    if (!parameters.contains("font") || !parameters.contains("style")
        || !parameters.contains("colorscheme") || !parameters.contains("cursortheme")
        || !parameters.contains("background") || !parameters.contains("rectangle")) {
        return KAuthorization::HelperError;
    }

    QString colorscheme = parameters.value("colorscheme").toString();
    if (colorscheme == QLatin1String("default")) {
        colorscheme = QString();
    }
    QString cursortheme = parameters.value("cursortheme").toString();
    if (cursortheme == QLatin1String("default")) {
        cursortheme = QString();
    }

    QSettings kgreetersettings(KDE_SYSCONFDIR "/lightdm/lightdm-kgreeter-greeter.conf", QSettings::IniFormat);
    kgreetersettings.setValue("greeter/font", parameters.value("font"));
    kgreetersettings.setValue("greeter/style", parameters.value("style"));
    kgreetersettings.setValue("greeter/colorscheme", colorscheme);
    kgreetersettings.setValue("greeter/cursortheme", cursortheme);
    kgreetersettings.setValue("greeter/background", parameters.value("background"));
    kgreetersettings.setValue("greeter/rectangle", parameters.value("rectangle"));
    if (kgreetersettings.status() != QSettings::NoError) {
        kWarning() << "Could not save settings";
        return KAuthorization::HelperError;
    }

    return KAuthorization::NoError;
}

K_AUTH_MAIN("org.kde.kcontrol.kcmkgreeter", KGreeterHelper)
