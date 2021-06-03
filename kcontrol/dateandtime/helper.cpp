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
#include <kauthhelpersupport.h>
#include <QProcess>
#include <QFile>
#include <QDir>

static QString findNtpUtility()
{
    foreach(const QString &possible_ntputility, QStringList() << "ntpdate" << "rdate" ) {
        const QString ntpUtility = KStandardDirs::findRootExe(possible_ntputility);
        if (!ntpUtility.isEmpty()) {
            return ntpUtility;
        }
    }
    return QString();
}

ClockHelper::CH_Error ClockHelper::ntp( const QStringList& ntpServers, bool ntpEnabled )
{
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

    if ( !QProcess::execute(ntpUtility, QStringList() << timeServer) ) {
      return NTPError;
    }
  } else if( ntpEnabled ) {
    return NTPError;
  }

  return NoError;
}

ClockHelper::CH_Error ClockHelper::date( const QString& newdate, const QString& olddate )
{
    struct timeval tv;

    tv.tv_sec = newdate.toULong() - olddate.toULong() + time(0);
    tv.tv_usec = 0;
    if (settimeofday(&tv, 0)) {
        return DateError;
    }

    QString hwclock = KStandardDirs::findRootExe("hwclock");
    if (!hwclock.isEmpty()) {
        QProcess::execute(hwclock, QStringList() << "--systohc");
    }
    return NoError;
}

ClockHelper::CH_Error ClockHelper::tz( const QString& selectedzone )
{
    //only allow letters, numbers hyphen underscore plus and forward slash
    //allowed pattern taken from time-util.c in systemd
    if (!QRegExp("[a-zA-Z0-9-_+/]*").exactMatch(selectedzone)) {
        return TimezoneError;
    }

    // NOTE: keep in sync with ktimezoned/ktimezoned.cpp
    static const QStringList zoneDirs = QStringList()
        << QLatin1String("/share/zoneinfo")
        << QLatin1String("/lib/zoneinfo")
        << QLatin1String("/usr/share/zoneinfo")
        << QLatin1String("/usr/lib/zoneinfo")
        << QLatin1String("/usr/local/share/zoneinfo")
        << QLatin1String("/usr/local/lib/zoneinfo")
        << (KStandardDirs::installPath("kdedir") + QLatin1String("/share/zoneinfo"))
        << (KStandardDirs::installPath("kdedir") + QLatin1String("/lib/zoneinfo"));

    // /usr is kind of standard
    QString ZONE_INFO_DIR = "/usr/share/zoneinfo";

    foreach (const QString &zonedir, zoneDirs) {
        if (QDir(zonedir).exists()) {
            ZONE_INFO_DIR = zonedir;
        }
    }

    QString tz = ZONE_INFO_DIR + selectedzone;

    QFile f("/etc/localtime");
    if (f.exists() && !f.remove()) {
        return TimezoneError;
    }

    if (!QFile::link(tz, "/etc/localtime")) {
        return TimezoneError;
    }

    QString val = ':' + tz;

    setenv("TZ", val.toAscii(), 1);
    tzset();

    return NoError;
}

ClockHelper::CH_Error ClockHelper::tzreset()
{
    unlink( "/etc/localtime" );

    setenv("TZ", "", 1);
    tzset();
    return NoError;
}

ActionReply ClockHelper::save(const QVariantMap &args)
{
  bool _ntp = args.value("ntp").toBool();
  bool _date = args.value("date").toBool();
  bool _tz = args.value("tz").toBool();
  bool _tzreset = args.value("tzreset").toBool();

  KComponentData data( "kcmdatetimehelper" );

  int ret = NoError; // error code
//  The order here is important
  if( _ntp )
    ret |= ntp( args.value("ntpServers").toStringList(), args.value("ntpEnabled").toBool());
  if( _date )
    ret |= date( args.value("newdate").toString(), args.value("olddate").toString() );
  if( _tz )
    ret |= tz( args.value("tzone").toString() );
  if( _tzreset )
    ret |= tzreset();

  if (ret == NoError) {
    return ActionReply::SuccessReply;
  } else {
    ActionReply reply(ActionReply::HelperError);
    reply.setErrorCode(ret);
    return reply;
  }
}

KDE4_AUTH_HELPER_MAIN("org.kde.kcontrol.kcmclock", ClockHelper)
