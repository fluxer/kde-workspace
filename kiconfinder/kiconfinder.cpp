/*  This file is part of the KDE project
    Copyright (C) 2008 David Faure <faure@kde.org>
    Copyright (C) 2015 Ivailo Monev <xakepa10@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License or ( at
    your option ) version 3 or, at the discretion of KDE e.V. ( which shall
    act as a proxy as in section 14 of the GPLv3 ), any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kcmdlineargs.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <klocale.h>
#include <kapplication.h>

#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[])
{
    KCmdLineArgs::init(
        argc, argv,
        "kiconfinder",
        0,
        ki18n("Icon Finder"),
        KDE_VERSION_STRING,
        ki18n("Finds an icon based on its name")
    );

    KCmdLineOptions options;
    options.add("+iconname", ki18n("The icon name to look for"));
    options.add("icongroup <group>", ki18n("The icon group to look in"), "desktop");
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count() < 1) {
        args->usage();
    }

    QStringList iconArgs;
    iconArgs.reserve(args->count());
    for (int i = 0; i < args->count(); i++) {
        iconArgs << args->arg(i);
    }
    const QString groupArg = args->getOption("icongroup");

    KIconLoader::Group iconGroup;
    if (groupArg == QLatin1String("none")) {
        iconGroup = KIconLoader::NoGroup;
    } else if (groupArg == QLatin1String("desktop")) {
        iconGroup = KIconLoader::Desktop;
    } else if(groupArg == QLatin1String("first")) {
        iconGroup = KIconLoader::FirstGroup;
    } else if(groupArg == QLatin1String("toolbar")) {
        iconGroup = KIconLoader::MainToolbar;
    } else if(groupArg == QLatin1String("small")) {
        iconGroup = KIconLoader::Small;
    } else if(groupArg == QLatin1String("panel")) {
        iconGroup = KIconLoader::Panel;
    } else if(groupArg == QLatin1String("dialog")) {
        iconGroup = KIconLoader::Dialog;
    } else if(groupArg == QLatin1String("last")) {
        iconGroup = KIconLoader::LastGroup;
    } else if(groupArg == QLatin1String("user")) {
        iconGroup = KIconLoader::User;
    } else {
        std::cerr << "Invalid icon group '" << groupArg.toLatin1().constData() << "," << std::endl;
        std::cerr << "Choose one of: none, desktop, first, toolbar, small, panel, dialog, last, user." << std::endl;
        return 1;
    }

    int rv = 0;
    foreach (const QString &iconName, iconArgs) {
        const QString icon = KIconLoader::global()->iconPath(iconName, iconGroup, true);
        if (!icon.isEmpty()) {
            const QByteArray iconBytes = icon.toLatin1();
            printf("%s\n", iconBytes.constData());
        } else {
            const QByteArray iconNameBytes = iconName.toLatin1();
            std::cerr << "Icon '" << iconNameBytes.constData() << "' not found" << std::endl;
            rv = 1;
        }
    }

    return rv;
}
