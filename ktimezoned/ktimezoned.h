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
class QFile;

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
        void  localChanged(const QString& path);

    private:
        // How the local time zone is specified
        enum LocalMethod
        {
            Utc,            // use UTC default: no local zone spec was found
            EnvTz,          // specified in TZ environment variable
            Localtime,      // specified in /etc/localtime
        };

        /** reimp */
        void  init(bool restart);
        bool  findZoneTab();
        bool  readZoneTab();
        void  findLocalZone();
        bool  checkEnv(const char *envZone);
        bool  checkLocaltime(const QString &path);
        void  updateLocalZone();
        bool  setLocalZone(const QString &path);

        QString     mZoneinfoDir;       // path to zoneinfo directory
        QString     mZoneTab;           // path to zone.tab file
        KSystemTimeZoneSource *mSource;
        KTimeZones  mZones;             // time zones collection
        QString     mLocalZoneDataFile; // zoneinfo file containing local time zone definition
        KDirWatch  *mZonetabWatch;      // watch for zone.tab file changes
        KDirWatch  *mDirWatch;          // watch for time zone definition file changes
        bool        mHaveCountryCodes;  // true if zone.tab contains any country codes
};

#endif
