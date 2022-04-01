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
#include <kauthhelpersupport.h>

#include "config-workspace.h"

ActionReply KGreeterHelper::save(const QVariantMap &parameters)
{
    if (!parameters.contains("style") || !parameters.contains("colorscheme")
        || !parameters.contains("background") || !parameters.contains("rectangle")) {
        return KAuth::ActionReply::HelperErrorReply;
    }

    QSettings kgreetersettings(KDE_SYSCONFDIR "/lightdm/lightdm-kgreeter-greeter.conf", QSettings::IniFormat);
    kgreetersettings.setValue("greeter/style", parameters.value("style"));
    kgreetersettings.setValue("greeter/colorscheme", parameters.value("colorscheme"));
    kgreetersettings.setValue("greeter/background", parameters.value("background"));
    kgreetersettings.setValue("greeter/rectangle", parameters.value("rectangle"));
    if (kgreetersettings.status() != QSettings::NoError) {
        KAuth::ActionReply errorreply(KAuth::ActionReply::HelperError);
        errorreply.setErrorDescription("Could not save settings");
        errorreply.setErrorCode(1);
        return errorreply;
    }

    return KAuth::ActionReply::SuccessReply;
}

KDE4_AUTH_HELPER_MAIN("org.kde.kcontrol.kcmkgreeter", KGreeterHelper)
