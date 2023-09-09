/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#include <QDBusInterface>
#include <QCoreApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KMountPoint>
#include <KDebug>

int main(int argc, char **argv) {
    KAboutData aboutData(
        "solid_unmount_device", 0, ki18n("solid_unmount_device"),
        "1.0.0", ki18n("Device unmounter for KDE."),
        KAboutData::License_GPL_V2,
        ki18n("(c) 2023 Ivailo Monev")
    );

    aboutData.addAuthor(ki18n("Ivailo Monev"), ki18n("Maintainer"), "xakepa10@gmail.com");
    aboutData.setProgramIconName(QLatin1String("media-eject"));

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions option;
    option.add("+[device]", ki18n("device to be unmounted"));
    KCmdLineArgs::addCmdLineOptions(option);

    QCoreApplication kapplication(argc, argv);
    QDBusInterface soliduiserver("org.kde.kded", "/modules/soliduiserver", "org.kde.SolidUiServer");
    const KMountPoint::List mountpoints = KMountPoint::currentMountPoints();
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for (int pos = 0; pos < args->count(); pos++) {
        const QString argstring = args->arg(pos);
        const KMountPoint::Ptr mp = mountpoints.findByDevice(argstring);
        if (mp && !mp->mountPoint().isEmpty()) {
            kDebug() << "Unmounting" << argstring;
            soliduiserver.call("unmountDevice", argstring);
        }
    }

    kDebug() << "All done";
    return 0;
}
