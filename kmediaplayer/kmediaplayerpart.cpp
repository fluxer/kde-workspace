/*  This file is part of the KDE libraries
    Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>

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

#include "kmediaplayerpart.h"

#include <KAboutData>
#include <KLocale>
#include <KMessageBox>
#include <KPluginFactory>
#include <KDebug>

K_PLUGIN_FACTORY(KMediaPlayerPartFactory, registerPlugin<KMediaPlayerPart>();)  // produce a factory
K_EXPORT_PLUGIN(KMediaPlayerPartFactory(KAboutData(
               "kmediaplayerpart",
               0,
               ki18n("KMediaPlayerPart"),
               "1.1.0",
               ki18n("Simple media player part for KDE."),
               KAboutData::License_GPL_V2,
               ki18n("(c) 2016 Ivailo Monev"),
               KLocalizedString(),
               "https://osdn.net/projects/kde/",
               "xakepa10@gmail.com").
               setProgramIconName(QLatin1String("KMediaPlayerPart")).
               setCatalogName("kmediaplayer")))

KMediaPlayerPart::KMediaPlayerPart(QWidget *parentWidget, QObject *parent, const QList<QVariant> &arguments)
    : KParts::ReadOnlyPart(parent)
    , m_player(new KMediaWidget(parentWidget, KMediaWidget::HiddenControls))
{
    Q_UNUSED(arguments);
    setObjectName(QString::fromLatin1("KMediaPlayerPart"));
    setComponentData(KMediaPlayerPartFactory::componentData());
    setWidget(m_player);
    setAutoDeleteWidget(false); // will be deleted in destructor, if still valid
    m_player->player()->setPlayerID("kmediaplayerpart");
}

KMediaPlayerPart::~KMediaPlayerPart()
{
    if (m_player) {
        delete m_player.data();
    }
}

bool KMediaPlayerPart::openUrl(const KUrl &url)
{
    setUrl(url);
    return openFile();
}

bool KMediaPlayerPart::openFile()
{
    const KUrl kurl = url();
    const QString kurlstring = kurl.prettyUrl();
    if (!kurl.isValid()) {
        KMessageBox::information(widget(), i18n("The URL is not valid: %1", kurlstring));
    } else if (!m_player->player()->isProtocolSupported(kurl.protocol())) {
        KMessageBox::information(widget(), i18n("The URL protocol is not valid: %1", kurl.protocol()));
    } else {
        m_player->open(kurlstring);
        return true;
    }

    return false;
}

bool KMediaPlayerPart::closeUrl()
{
    m_player->player()->stop();
    return ReadOnlyPart::closeUrl();
}

#include "moc_kmediaplayerpart.cpp"
