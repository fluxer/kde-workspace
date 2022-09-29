/*
   Copyright (C) 2007 by Olivier Goffart <ogoffart at kde.org>
   Copyright (C) 2009 by Laurent Montel <montel@kde.org>


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
#include "notifybyktts.h"
#include <QtDBus/QtDBus>
#include <QHash>
#include <ktoolinvocation.h>
#include <kmessagebox.h>
#include <kmacroexpander.h>
#include <klocale.h>
#include <knotifyconfig.h>

NotifyByKTTS::NotifyByKTTS(QObject *parent)
    : KNotifyPlugin(parent),
    m_kspeech(0)
{
}


NotifyByKTTS::~NotifyByKTTS()
{
}

void NotifyByKTTS::notify( int id, KNotifyConfig * config )
{
    if (  !KSpeech::isSupported() )
        return;

    if (!m_kspeech) {
        m_kspeech = new KSpeech(this);
    }

    QString say = config->readEntry( "KTTS" );
    QString appId = config->appname;
    if (appId.isEmpty()) {
        appId = QString::fromLatin1("KNotify");
    }

    if (!say.isEmpty()) {
        QHash<QChar,QString> subst;
        subst.insert( 'e', config->eventid );
        subst.insert( 'a', config->appname );
        subst.insert( 's', config->text );
        subst.insert( 'w', QString::number( (quintptr)config->winId ));
        subst.insert( 'i', QString::number( id ));
        subst.insert( 'm', config->text );
        say = KMacroExpander::expandMacrosShellQuote( say, subst );
    }

    if ( say.isEmpty() )
        say = config->text; // fallback

    m_kspeech->setSpeechID(appId);
    m_kspeech->say(say);

    finished(id);
}

#include "moc_notifybyktts.cpp"
