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

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kfilemetainfo.h>
#include <kurl.h>
#include <klocale.h>
#include <kdebug.h>

int main(int argc, char **argv)
{
    KAboutData aboutData("kfile", 0, ki18n("kfile4"),
                         "1.0.0", ki18n("A command-line tool to read metadata of files."),
                         KAboutData::License_GPL_V2,
                         ki18n("(c) 2022 Ivailo Monev"),
                         KLocalizedString(),
                        "http://github.com/fluxer/katana"
                        );
    aboutData.addAuthor(ki18n("Ivailo Monev"),
                        ki18n("Maintainer"),
                        "xakepa10@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("ls"); // short option for --listsupported
    options.add("listsupported", ki18n("List all supported metadata keys." ));
    options.add("la"); // short option for --listavailable
    options.add("listavailable", ki18n("List all metadata keys which have a value in the given URL(s)."));
    options.add("av"); // short option for --allValues
    options.add("allvalues", ki18n("Prints all metadata values, available in the given URL(s)."));
    options.add("getvalue <key>", ki18n("Prints the value for 'key' of the given URL(s). 'key' may also be a comma-separated list of keys"));
    options.add("+[url]", ki18n("The URL(s) to operate on."));
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->count() == 0) {
        // exits
        KCmdLineArgs::usageError(i18n("No URL specified"));
    }

    const bool listsupported = (args->isSet("ls") || args->isSet("listsupported"));
    const bool listavailable = (args->isSet("la") || args->isSet("listavailable"));
    const bool allvalues = (args->isSet("av") || args->isSet("allvalues"));
    const bool getvalue = args->isSet("getvalue");
    if (!listsupported && !listavailable && !allvalues && !getvalue) {
        KCmdLineArgs::usageError(i18n("One of listsupported, listavailable, allvalues or getvalue must be specified"));
    }

    for (int pos = 0; pos < args->count(); ++pos) {
        const KUrl url = args->url(pos);
        const KFileMetaInfo metainfo(url, KFileMetaInfo::Everything);
        if (listsupported) {
            foreach (const QString &key, metainfo.supportedKeys()) {
                qDebug() << key;
            }
        } else if (listavailable) {
            foreach (const QString &key, metainfo.keys()) {
                qDebug() << key;
            }
        } else if (allvalues) {
            foreach (const KFileMetaInfoItem &item, metainfo.items().values()) {
                qDebug() << item.key() << item.value().toString();
            }
        } else if (getvalue) {
            const QString getvaluekey = args->getOption("getvalue");
            const KFileMetaInfoItem item = metainfo.item(getvaluekey);
            qDebug() << item.value().toString();
        } else {
            Q_ASSERT(false);
        }
    }

    return 0;
}
