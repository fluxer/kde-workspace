/*
   This file is part of the KDE libraries
   Copyright (c) 2005-2010 David Jarvie <djarvie@kde.org>
   Copyright (c) 2005 S.R.Haque <srhaque@iee.org>.

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

#include "ktimezoned.moc"
#include "ktimezonedbase.moc"

#include <climits>
#include <cstdlib>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QtDBus/QtDBus>

#include <kglobal.h>
#include <klocale.h>
#include <kcodecs.h>
#include <kstandarddirs.h>
#include <kstringhandler.h>
#include <ktemporaryfile.h>
#include <kdebug.h>
#include <kconfiggroup.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY(KTimeZonedFactory,
                 registerPlugin<KTimeZoned>();
    )
K_EXPORT_PLUGIN(KTimeZonedFactory("ktimezoned"))

// The maximum allowed length for reading a zone.tab line. This is set to
// provide plenty of leeway, given that the maximum length of lines in a valid
// zone.tab will be around 100 - 120 characters.
const int MAX_ZONE_TAB_LINE_LENGTH = 2000;

// Config file entry names
const char ZONEINFO_DIR[]   = "ZoneinfoDir";   // path to zoneinfo/ directory
const char ZONE_TAB[]       = "Zonetab";       // path & name of zone.tab
const char LOCAL_ZONE[]     = "LocalZone";     // name of local time zone


KTimeZoned::KTimeZoned(QObject* parent, const QList<QVariant>& l)
  : KTimeZonedBase(parent, l),
    mSource(0),
    mZonetabWatch(0),
    mDirWatch(0)
{
    init(false);
}

KTimeZoned::~KTimeZoned()
{
    delete mSource;
    mSource = 0;
    delete mZonetabWatch;
    mZonetabWatch = 0;
    delete mDirWatch;
    mDirWatch = 0;
}

void KTimeZoned::init(bool restart)
{
    if (restart)
    {
        kDebug(1221) << "KTimeZoned::init(restart)";
        delete mSource;
        mSource = 0;
        delete mZonetabWatch;
        mZonetabWatch = 0;
        delete mDirWatch;
        mDirWatch = 0;
    }

    KConfig config(QLatin1String("ktimezonedrc"));
    if (restart)
        config.reparseConfiguration();
    KConfigGroup group(&config, "TimeZones");
    mZoneinfoDir     = group.readEntry(ZONEINFO_DIR);
    mZoneTab         = group.readEntry(ZONE_TAB);
    mConfigLocalZone = group.readEntry(LOCAL_ZONE);
    if (mZoneinfoDir.length() > 1 && mZoneinfoDir.endsWith('/'))
        mZoneinfoDir.truncate(mZoneinfoDir.length() - 1);   // strip trailing '/'

    // Open zone.tab if we already know where it is
    QFile f;
    // Search for zone.tab
    if (!findZoneTab(f))
        return;
    mZoneTab = f.fileName();

    // Read zone.tab and create a collection of KTimeZone instances
    readZoneTab(f);

    mZonetabWatch = new KDirWatch(this);
    mZonetabWatch->addFile(mZoneTab);
    connect(mZonetabWatch, SIGNAL(dirty(const QString&)), SLOT(zonetab_Changed(const QString&)));

    // Respect the config above all
    if (!mConfigLocalZone.isEmpty()) {
        // We know the local zone from last time.
        // Check whether the file still matches it.
        KTimeZone local = mZones.zone(mConfigLocalZone);
        if (local.isValid())
        {
            mLocalZone = mConfigLocalZone;
            mLocalZoneDataFile = mZoneinfoDir + "/" + mConfigLocalZone;
        }
    }

    // Find the local system time zone and set up file monitors to detect changes
    findLocalZone();
}

// Check if the local zone has been updated, and if so, notify interested parties.
void KTimeZoned::updateLocalZone()
{
    qDebug() << "updating timezone" << mLocalZone;
    KConfig config(QLatin1String("ktimezonedrc"));
    KConfigGroup group(&config, "TimeZones");
    mConfigLocalZone = mLocalZone;
    group.writeEntry(LOCAL_ZONE, mConfigLocalZone);
    group.sync();

    QDBusMessage message = QDBusMessage::createSignal("/Daemon", "org.kde.KTimeZoned", "configChanged");
    QDBusConnection::sessionBus().send(message);
}

/*
 * Find the location of the zoneinfo files and store in mZoneinfoDir.
 * Open or if necessary create zone.tab.
 */
bool KTimeZoned::findZoneTab(QFile& f)
{
    QString ZONE_TAB_FILE = "/zone.tab";
    QString ZONE_INFO_DIR;

    if (QDir("/usr/share/zoneinfo").exists()) {
        ZONE_INFO_DIR = "/usr/share/zoneinfo";
    } else if (QDir("/usr/lib/zoneinfo").exists()) {
        ZONE_INFO_DIR = "/usr/lib/zoneinfo";
    } else if (QDir("/share/zoneinfo").exists()) {
        ZONE_INFO_DIR = "/share/zoneinfo";
    } else if (QDir("/lib/zoneinfo").exists()) {
        ZONE_INFO_DIR = "/lib/zoneinfo";
    } else {
        // /usr is kind of standard
        ZONE_INFO_DIR = "/usr/share/zoneinfo";
    }

    // Find and open zone.tab - it's all easy except knowing where to look.
    QDir dir;
    QString zoneinfoDir = ZONE_INFO_DIR;
    // make a note if the dir exists; whether it contains zone.tab or not
    if (dir.exists(zoneinfoDir))
    {
        mZoneinfoDir = zoneinfoDir;
        f.setFileName(zoneinfoDir + ZONE_TAB_FILE);
        if (f.open(QIODevice::ReadOnly))
            mZoneTab = zoneinfoDir + ZONE_TAB_FILE;
            return true;
        kDebug(1221) << "Can't open " << f.fileName();
    }

    zoneinfoDir = ::getenv("TZDIR");
    if (!zoneinfoDir.isEmpty() && dir.exists(zoneinfoDir))
    {
        mZoneinfoDir = zoneinfoDir;
        f.setFileName(zoneinfoDir + ZONE_TAB_FILE);
        if (f.open(QIODevice::ReadOnly))
            mZoneTab = zoneinfoDir + ZONE_TAB_FILE;
            return true;
        kDebug(1221) << "Can't open " << f.fileName();
    }

    return false;
}

// Parse zone.tab and for each time zone, create a KSystemTimeZone instance.
// Note that only data needed by this module is specified to KSystemTimeZone.
void KTimeZoned::readZoneTab(QFile &f)
{
    qDebug() << "reading zone.tab";
    // Parse the already open real or fake zone.tab.
    QRegExp lineSeparator("[ \t]");
    if (!mSource)
        mSource = new KSystemTimeZoneSource;
    mZones.clear();
    QTextStream str(&f);
    while (!str.atEnd())
    {
        // Read the next line, but limit its length to guard against crashing
        // due to a corrupt very large zone.tab (see KDE bug 224868).
        QString line = str.readLine(MAX_ZONE_TAB_LINE_LENGTH);
        if (line.isEmpty() || line[0] == '#')
            continue;
        QStringList tokens = KStringHandler::perlSplit(lineSeparator, line, 4);
        int n = tokens.count();
        if (n < 3)
        {
            kError(1221) << "readZoneTab(): invalid record: " << line << endl;
            continue;
        }

        // Add entry to list.
        if (tokens[0] == "??")
            tokens[0] = "";
        else if (!tokens[0].isEmpty())
            mHaveCountryCodes = true;
        mZones.add(KSystemTimeZone(mSource, tokens[2], tokens[0]));
    }
    f.close();
}

// Find the local time zone, starting from scratch.
void KTimeZoned::findLocalZone()
{
    delete mDirWatch;
    mDirWatch = 0;
    mLocalZone.clear();
    mLocalZoneDataFile.clear();

    // First try the checking for TZ envinomental variable.
    const char *envtz = ::getenv("TZ");
    checkEnv(envtz);

    if (mLocalZone.isEmpty() && !mZoneinfoDir.isEmpty()) {
        // Try to match /etc/localtime against the list of zoneinfo files.
        // Resolve symlink
        QFileInfo fi(QFile("/etc/localtime"));
        if (fi.isSymLink()) {
            checkLocaltime(fi.canonicalFilePath());
        } else {
            checkLocaltime(QLatin1String("/etc/localtime"));
        }
    }

    // Failsafe, fallback to UTC
    if (mLocalZone.isEmpty()) {
        mLocalZone = KTimeZone::utc().name();
        kDebug(1221) << "Failsafe: " << mLocalZone;
    }

    // Watch for changes in the file defining the local time zone
    mDirWatch = new KDirWatch(this);
    mDirWatch->addFile("/etc/localtime");
    if (!mLocalZoneDataFile.isEmpty())
        mDirWatch->addFile(mLocalZoneDataFile);
        qDebug() << "watching" << mLocalZoneDataFile;
    connect(mDirWatch, SIGNAL(dirty(const QString&)), SLOT(localChanged(const QString&)));
    connect(mDirWatch, SIGNAL(deleted(const QString&)), SLOT(localChanged(const QString&)));
    connect(mDirWatch, SIGNAL(created(const QString&)), SLOT(localChanged(const QString&)));
    mDirWatch->startScan();

    // Finally, if the local zone identity has changed, let the world know
    updateLocalZone();
}

// Called when KDirWatch detects a change in zone.tab
void KTimeZoned::zonetab_Changed(const QString& path)
{
    qDebug() << "zone.tab changed";
    if (path != mZoneTab) {
        kError(1221) << "Wrong path (" << path << ") for zone.tab";
        return;
    }
    QDBusMessage message = QDBusMessage::createSignal("/Daemon", "org.kde.KTimeZoned", "zonetabChanged");
    QList<QVariant> args;
    args += mZoneTab;
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);

    // Reread zone.tab and recreate the collection of KTimeZone instances,
    // in case any zones have been created or deleted and one of them
    // subsequently becomes the local zone.
    QFile f;
    f.setFileName(mZoneTab);
    if (!f.open(QIODevice::ReadOnly)) {
        kError(1221) << "Could not open zone.tab (" << mZoneTab << ") to reread";
    } else {
        readZoneTab(f);
    }
}

// Called when KDirWatch detects a change
void KTimeZoned::localChanged(const QString& path)
{
    kDebug(1221) << path << "changed";
    findLocalZone();
}

bool KTimeZoned::checkEnv(const char *envZone)
{
    if (!mLocalZone.isEmpty()) {
        // It has most likely already been set by preferences in the config
        return true;
    }
    if (envZone) {
        if (envZone[0] == ':') {
            // TZ specified, either relative to zoneinfo/ or absolute.
            QString tz = QFile::decodeName(envZone + 1);
            KTimeZone local = mZones.zone(tz);
            if (!local.isValid()) {
                return false;
            }
            mLocalZone = tz;
            if (QFile(tz).exists()) {
                // Full path.
                mLocalZoneDataFile = tz;
            } else {
                // Relative path.
                mLocalZoneDataFile = mZoneinfoDir + "/" + tz;
            }
        }
    }
    return false;
}

bool KTimeZoned::checkLocaltime(const QString &path)
{
    if (!mLocalZone.isEmpty()) {
        // It has most likely already been set by preferences in the config
        return true;
    }

    if (mZoneinfoDir.isEmpty() && !path.indexOf(mZoneinfoDir)) {
        qDebug() << "relative path passed to checkLocaltime()";
        return false;
    }

    // The path could be full path, remove zoneinfo directory reference
    QString rel = path.mid(mZoneinfoDir.length() + 1);

    KTimeZone local = mZones.zone(rel);
    if (!local.isValid()) {
        return false;
    }

    mLocalZone = rel;
    mLocalZoneDataFile = path;
    kDebug(1221) << mLocalZone;
    return true;
}
