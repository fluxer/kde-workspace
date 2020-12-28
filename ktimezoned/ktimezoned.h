/*
   This file is part of the KDE libraries
   Copyright (c) 2007,2009 David Jarvie <djarvie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KTIMEZONED_H
#define KTIMEZONED_H

#include "ktimezonedbase.h"

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QTimer>

#include <kdedmodule.h>
#include <kdirwatch.h>
#include <ksystemtimezone.h>


class KTimeZoned : public KTimeZonedBase
{
        Q_OBJECT

    public:
        KTimeZoned(QObject* parent, const QList<QVariant>&);
        ~KTimeZoned();

    private Q_SLOTS:
        void  zonetab_Changed(const QString& path);
        void  localChanged();

    private:
        /** reimp */
        void  init(bool restart);
        bool  findZoneTab(QFile& f);
        void  readZoneTab(QFile& f);
        void  findLocalZone();
        bool  checkTZ(const char *envZone);
        bool  checkLocaltimeLink();
        bool  checkLocaltimeFile();
        bool  checkTimezone();
        void  updateLocalZone();
        bool  matchZoneFile(const QString &path);
        bool  setLocalZone(const QString &zoneName);

        QString     mZoneinfoDir;       // path to zoneinfo directory
        QString     mZoneTab;           // path to zone.tab file
        QByteArray  mSavedTZ;           // last value of TZ if it's used to set local zone
        KSystemTimeZoneSource *mSource;
        KTimeZones  mZones;             // time zones collection
        QString     mLocalIdFile;       // file containing pointer to local time zone definition
        QString     mLocalZoneDataFile; // zoneinfo file containing local time zone definition
        KDirWatch  *mZonetabWatch;      // watch for zone.tab file changes
        QTimer     *mPollWatch;         // watch for time zone definition file changes
        bool        mHaveCountryCodes;  // true if zone.tab contains any country codes
        QDateTime   mLocalStamp;        // mLocalIdFile modification time
};

#endif
