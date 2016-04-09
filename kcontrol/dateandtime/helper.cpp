/*
 *  tzone.cpp
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*

 A helper that's run using KAuth and does the system modifications.

*/

#include "helper.h"

#include <config-workspace.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include <kcomponentdata.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kauthhelpersupport.h>
#include <QFile>
#include <QDir>

// We cannot rely on the $PATH environment variable, because D-Bus activation
// clears it. So we have to use a reasonable default.
static const QString exePath = QLatin1String("/usr/sbin:/usr/bin:/sbin:/bin");

static QString findNtpUtility()
{
    foreach(const QString &possible_ntputility, QStringList() << "ntpdate" << "rdate" ) {
        const QString ntpUtility = KStandardDirs::findExe(possible_ntputility, exePath);
        if (!ntpUtility.isEmpty()) {
            return ntpUtility;
        }
    }
    return QString();
}

int ClockHelper::ntp( const QStringList& ntpServers, bool ntpEnabled )
{
  int ret = 0;

  // write to the system config file
  QFile config_file(KDE_CONFDIR "/kcmclockrc");
  if(!config_file.exists()) {
    config_file.open(QIODevice::WriteOnly);
    config_file.close();
    config_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
  }
  KConfig _config(config_file.fileName(), KConfig::SimpleConfig);
  KConfigGroup config(&_config, "NTP");
  config.writeEntry("servers", ntpServers );
  config.writeEntry("enabled", ntpEnabled );

  QString ntpUtility(findNtpUtility());

  if ( ntpEnabled && !ntpUtility.isEmpty() ) {
    // NTP Time setting
    QString timeServer = ntpServers.first();
    if( timeServer.indexOf( QRegExp(".*\\(.*\\)$") ) != -1 ) {
      timeServer.replace( QRegExp(".*\\("), "" );
      timeServer.replace( QRegExp("\\).*"), "" );
      // Would this be better?: s/^.*\(([^)]*)\).*$/\1/
    }

    KProcess proc;
    proc << ntpUtility << timeServer;
    if ( proc.execute() != 0 ) {
      ret |= NTPError;
    }
  } else if( ntpEnabled ) {
    ret |= NTPError;
  }

  return ret;
}

int ClockHelper::date( const QString& newdate, const QString& olddate )
{
    struct timeval tv;

    tv.tv_sec = newdate.toULong() - olddate.toULong() + time(0);
    tv.tv_usec = 0;
    if (settimeofday(&tv, 0)) {
        return DateError;
    }

    QString hwclock = KStandardDirs::findExe("hwclock", exePath);
    if (!hwclock.isEmpty()) {
        KProcess::execute(hwclock, QStringList() << "--systohc");
    }
    return 0;
}

int ClockHelper::tz( const QString& selectedzone )
{
    int ret = 0;

    //only allow letters, numbers hyphen underscore plus and forward slash
    //allowed pattern taken from time-util.c in systemd
    if (!QRegExp("[a-zA-Z0-9-_+/]*").exactMatch(selectedzone)) {
        return ret;
    }
        // from ktimezoned
        QString ZONE_INFO_DIR;

        if (QDir("/usr/share/zoneinfo").exists()) {
            ZONE_INFO_DIR = "/usr/share/zoneinfo/";
        } else if (QDir("/usr/lib/zoneinfo").exists()) {
            ZONE_INFO_DIR = "/usr/lib/zoneinfo/";
        } else if (QDir("/share/zoneinfo").exists()) {
            ZONE_INFO_DIR = "/share/zoneinfo/";
        } else if (QDir("/lib/zoneinfo").exists()) {
            ZONE_INFO_DIR = "/lib/zoneinfo/";
        } else {
            // /usr is kind of standard
            ZONE_INFO_DIR = "/usr/share/zoneinfo/";
        }
        QString tz = ZONE_INFO_DIR + selectedzone;

        QFile f("/etc/localtime");
        if (f.exists() && !f.remove()) {
          ret |= TimezoneError;
        }
        if (!QFile::link(tz, "/etc/localtime")) {
          ret |= TimezoneError;
        }

        QString val = ':' + tz;

        setenv("TZ", val.toAscii(), 1);
        tzset();

    return ret;
}

int ClockHelper::tzreset()
{
    unlink( "/etc/localtime" );

    setenv("TZ", "", 1);
    tzset();
    return 0;
}

ActionReply ClockHelper::save(const QVariantMap &args)
{
  bool _ntp = args.value("ntp").toBool();
  bool _date = args.value("date").toBool();
  bool _tz = args.value("tz").toBool();
  bool _tzreset = args.value("tzreset").toBool();

  KComponentData data( "kcmdatetimehelper" );

  int ret = 0; // error code
//  The order here is important
  if( _ntp )
    ret |= ntp( args.value("ntpServers").toStringList(), args.value("ntpEnabled").toBool());
  if( _date )
    ret |= date( args.value("newdate").toString(), args.value("olddate").toString() );
  if( _tz )
    ret |= tz( args.value("tzone").toString() );
  if( _tzreset )
    ret |= tzreset();

  if (ret == 0) {
    return ActionReply::SuccessReply;
  } else {
    ActionReply reply(ActionReply::HelperError);
    reply.setErrorCode(ret);
    return reply;
  }
}

KDE4_AUTH_HELPER_MAIN("org.kde.kcontrol.kcmclock", ClockHelper)
