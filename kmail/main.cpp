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

#include <KAboutData>
#include <KCmdLineArgs>
#include <KApplication>
#include <KLocale>
#include <KEMailDialog>
#include <KHelpMenu>
#include <KDebug>

static QStringList splitMailArg(const QString &arg)
{
    return arg.split(QLatin1Char(','));
}

int main(int argc, char **argv) {
    KAboutData aboutData(
        "kmail", 0, ki18n("KMail"),
        "1.0.0", ki18n("Simple e-mail sender for KDE."),
        KAboutData::License_GPL_V2,
        ki18n("(c) 2022 Ivailo Monev"),
        KLocalizedString(),
        "https://osdn.net/projects/kde/"
    );

    aboutData.addAuthor(
        ki18n("Ivailo Monev"),
        ki18n("Maintainer"),
        "xakepa10@gmail.com"
    );
    aboutData.setProgramIconName(QLatin1String("internet-mail"));
    aboutData.setOrganizationDomain("kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions option;
    option.add("+[url]", ki18n("URL to be opened"));
    KCmdLineArgs::addCmdLineOptions(option);

    KApplication *kapplication = new KApplication();
    KEMailDialog kemaildialog;
    kemaildialog.show();
    kemaildialog.setButtons(KDialog::Ok | KDialog::Close | KDialog::Help);
    KHelpMenu khelpmenu(&kemaildialog, &aboutData, true);
    kemaildialog.setButtonMenu(KDialog::Help, (QMenu*)khelpmenu.menu());

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for (int pos = 0; pos < args->count(); pos++) {
        const KUrl argurl = args->url(pos);

        if (argurl.hasQueryItem("subject")) {
            kemaildialog.setSubject(argurl.queryItemValue("subject"));
        }

        QStringList mailto;
        if (argurl.hasQueryItem("to")) {
            mailto.append(splitMailArg(argurl.queryItemValue("to")));
        }
        if (argurl.hasQueryItem("cc")) {
            mailto.append(splitMailArg(argurl.queryItemValue("cc")));
        }
        if (mailto.isEmpty()) {
            mailto.append(argurl.path());
        }
        kemaildialog.setTo(mailto);

        if (argurl.hasQueryItem("body")) {
            kemaildialog.setMessage(argurl.queryItemValue("body"));
        }

        if (argurl.hasQueryItem("attach")) {
            kemaildialog.setAttach(splitMailArg(argurl.queryItemValue("attach")));
        }
        break;
    }

    return kapplication->exec();
}
