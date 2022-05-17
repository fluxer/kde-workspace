/*
   Copyright (c) 1997 Christian Esken (esken@kde.org)
                 2000 Charles Samuels (charles@kde.org)
                 2000 Stefan Schimanski (1Stein@gmx.de)
                 2000 Matthias Ettrich (ettrich@kde.org)
                 2000 Waldo Bastian <bastian@kde.org>
                 2000-2003 Carsten Pfeiffer <pfeiffer@kde.org>
                 2005 Allan Sandfeld Jensen <kde@carewolf.com>
                 2005-2006 by Olivier Goffart <ogoffart at kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "notifybysound.h"

// QT headers
#include <QtCore/QTimer>
#include <QtCore/QQueue>
#include <QtCore/qcoreevent.h>
#include <QtCore/QStack>
#include <QSignalMapper>

// KDE headers
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kconfiggroup.h>
#include <kurl.h>
#include <config-workspace.h>
#include <kcomponentdata.h>
#include <knotifyconfig.h>
#include <kmediaplayer.h>

class NotifyBySound::Private
{
public:
    bool noSound;
    QMap<int, KAudioPlayer*> playerObjects;
    QSignalMapper *signalmapper;
    QQueue<int> closeQueue;
};

NotifyBySound::NotifyBySound(QObject *parent) : KNotifyPlugin(parent),d(new Private)
{
    d->signalmapper = new QSignalMapper(this);
    connect(d->signalmapper, SIGNAL(mapped(int)), this, SLOT(slotSoundFinished(int)));

    loadConfig();
}


NotifyBySound::~NotifyBySound()
{
    delete d;
}


void NotifyBySound::loadConfig()
{
    // load player settings
    KSharedConfig::Ptr kc = KGlobal::config();
    KConfigGroup cg(kc, "Sounds");

    d->noSound = cg.readEntry( "No sound" , false );
}




void NotifyBySound::notify( int eventId, KNotifyConfig * config )
{
    if (d->noSound) {
        finish( eventId );
        return;
    }

    if (d->playerObjects.contains(eventId)) {
        //a sound is already playing for this notification,  we don't support playing two sounds.
        finish( eventId );
        return;
    }

    KUrl soundFileURL = config->readEntry( "Sound" , true );
    QString soundFile = soundFileURL.toLocalFile();

    if (soundFile.isEmpty()) {
        finish( eventId );
        return;
    }

    // get file name
    if (KUrl::isRelativeUrl(soundFile)) {
        QString search = QString("%1/sounds/%2").arg(config->appname).arg(soundFile);
        search = KGlobal::mainComponent().dirs()->findResource("data", search);
        if ( search.isEmpty() ) {
            soundFile = KStandardDirs::locate( "sound", soundFile );
        } else {
            soundFile = search;
        }
    }
    if ( soundFile.isEmpty() ) {
        finish( eventId );
        return;
    }

    kDebug() << " going to play " << soundFile;
    if (!d->noSound) {
        kDebug() << "creating new player";
        KAudioPlayer *player = new KAudioPlayer(this);
        player->setPlayerID("knotify");
        connect(player, SIGNAL(finished()), d->signalmapper, SLOT(map()));
        d->signalmapper->setMapping(player, eventId);
        d->playerObjects.insert(eventId, player);
        player->load(soundFile);
    }
}

void NotifyBySound::slotSoundFinished(int id)
{
    kDebug() << id;
    if (d->playerObjects.contains(id)) {
        KAudioPlayer *player = d->playerObjects.take(id);
        disconnect(player, SIGNAL(finished()), d->signalmapper, SLOT(map()));
        kDebug() << "destroying idle player";
        player->deleteLater();
    }
    finish(id);
}

void NotifyBySound::close(int id)
{
    // close in 1 min - ugly workaround for sounds getting cut off because the close call in kdelibs
    // is hardcoded to 6 seconds
    d->closeQueue.enqueue(id);
    QTimer::singleShot(60000, this, SLOT(closeNow()));
}

void NotifyBySound::closeNow()
{
    const int id = d->closeQueue.dequeue();
    if (d->playerObjects.contains(id)) {
        KAudioPlayer *player = d->playerObjects.value(id);
        player->stop();
    }
}

#include "moc_notifybysound.cpp"
// vim: ts=4 noet
