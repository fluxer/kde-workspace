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

#include "moc_ktimezoned.cpp"
#include "moc_ktimezonedbase.cpp"

#include <climits>
#include <cstdlib>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QStringList>
#include <QtDBus/QtDBus>

#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstringhandler.h>
#include <ktemporaryfile.h>
#include <kdebug.h>
#include <kconfiggroup.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY(KTimeZonedFactory, registerPlugin<KTimeZoned>();)
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
    mPollWatch(0)
{
    init(false);
}

KTimeZoned::~KTimeZoned()
{
    delete mSource;
    mSource = 0;
    delete mZonetabWatch;
    mZonetabWatch = 0;
    delete mPollWatch;
    mPollWatch = 0;
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
        delete mPollWatch;
        mPollWatch = 0;
    }

    KConfig config(QLatin1String("ktimezonedrc"));
    if (restart)
        config.reparseConfiguration();
    KConfigGroup group(&config, "TimeZones");
    mZoneinfoDir = QDir::cleanPath(group.readEntry(ZONEINFO_DIR));
    mZoneTab = group.readEntry(ZONE_TAB);
    mConfigLocalZone = group.readEntry(LOCAL_ZONE);
    if (mZoneinfoDir.length() > 1 && mZoneinfoDir.endsWith('/'))
        mZoneinfoDir.truncate(mZoneinfoDir.length() - 1);   // strip trailing '/'

    kDebug(1221) << ZONEINFO_DIR << mZoneinfoDir;
    kDebug(1221) << ZONE_TAB << mZoneTab;
    kDebug(1221) << LOCAL_ZONE << mConfigLocalZone;

    // For Unix, read zone.tab.

    QString oldZoneinfoDir = mZoneinfoDir;
    QString oldZoneTab     = mZoneTab;

    // Open zone.tab if we already know where it is
    QFile f;
    if (!mZoneTab.isEmpty() && !mZoneinfoDir.isEmpty())
    {
        f.setFileName(mZoneTab);
        if (!f.open(QIODevice::ReadOnly)) {
            mZoneTab.clear();
        }
    }

    if (mZoneTab.isEmpty() || mZoneinfoDir.isEmpty())
    {
        // Search for zone.tab
        if (!findZoneTab(f))
            return;
        mZoneTab = f.fileName();

        if (mZoneinfoDir != oldZoneinfoDir
        ||  mZoneTab != oldZoneTab)
        {
            // Update config file and notify interested applications
            group.writeEntry(ZONEINFO_DIR, mZoneinfoDir);
            group.writeEntry(ZONE_TAB, mZoneTab);
            group.sync();
            QDBusMessage message = QDBusMessage::createSignal("/Daemon", "org.kde.KTimeZoned", "configChanged");
            QDBusConnection::sessionBus().send(message);
        }
    }

    // Read zone.tab and create a collection of KTimeZone instances
    readZoneTab(f);

    mZonetabWatch = new KDirWatch(this);
    mZonetabWatch->addFile(mZoneTab);
    connect(mZonetabWatch, SIGNAL(dirty(const QString&)), SLOT(zonetab_Changed(const QString&)));
    mPollWatch = new QTimer();
    // NOTE: super nasty hack #2, see dateandtime KCM
    mPollWatch->start(3000);
    // Watch for changes in the file defining the local time zone so as to be
    // notified of any change in it.
    connect(mPollWatch, SIGNAL(timeout()), SLOT(localChanged()));


    // Find the local system time zone and set up file monitors to detect changes
    findLocalZone();
}

// Check if the local zone has been updated, and if so, write the new
// zone to the config file and notify interested parties.
void KTimeZoned::updateLocalZone()
{
    if (mConfigLocalZone != mLocalZone)
    {
        KConfig config(QLatin1String("ktimezonedrc"));
        KConfigGroup group(&config, "TimeZones");
        mConfigLocalZone = mLocalZone;
        group.writeEntry(LOCAL_ZONE, mConfigLocalZone);
        group.sync();

        QDBusMessage message = QDBusMessage::createSignal("/Daemon", "org.kde.KTimeZoned", "configChanged");
        QDBusConnection::sessionBus().send(message);
    }
}

/*
 * Find the location of the zoneinfo files and store in mZoneinfoDir.
 * Open or if necessary create zone.tab.
 */
bool KTimeZoned::findZoneTab(QFile& f)
{
    // NOTE: keep in sync with kcontrol/dateandtime/helper.cpp
    static const QStringList zoneDirs = QStringList()
        << QLatin1String("/share/zoneinfo")
        << QLatin1String("/lib/zoneinfo")
        << QLatin1String("/usr/share/zoneinfo")
        << QLatin1String("/usr/lib/zoneinfo")
        << QLatin1String("/usr/local/share/zoneinfo")
        << QLatin1String("/usr/local/lib/zoneinfo")
        << (KStandardDirs::installPath("kdedir") + QLatin1String("/share/zoneinfo"))
        << (KStandardDirs::installPath("kdedir") + QLatin1String("/lib/zoneinfo"));

    QString ZONE_TAB_FILE = "/zone.tab";
    // /usr is kind of standard
    QString ZONE_INFO_DIR = "/usr/share/zoneinfo";

    foreach (const QString &zonedir, zoneDirs) {
        if (QDir(zonedir).exists()) {
            ZONE_INFO_DIR = zonedir;
            break;
        }
    }

    // Find and open zone.tab - it's all easy except knowing where to look.
    QDir dir;
    QString zoneinfoDir = ZONE_INFO_DIR;
    // make a note if the dir exists; whether it contains zone.tab or not
    if (dir.exists(zoneinfoDir))
    {
        mZoneinfoDir = QDir::cleanPath(zoneinfoDir);
        f.setFileName(mZoneinfoDir + ZONE_TAB_FILE);
        if (f.open(QIODevice::ReadOnly))
            return true;
        kDebug(1221) << "Can't open " << f.fileName();
    }

    zoneinfoDir = ::getenv("TZDIR");
    if (!zoneinfoDir.isEmpty() && dir.exists(zoneinfoDir))
    {
        mZoneinfoDir = QDir::cleanPath(zoneinfoDir);
        f.setFileName(mZoneinfoDir + ZONE_TAB_FILE);
        if (f.open(QIODevice::ReadOnly))
            return true;
        kDebug(1221) << "Can't open " << f.fileName();
    }

    return false;
}

// Parse zone.tab and for each time zone, create a KSystemTimeZone instance.
// Note that only data needed by this module is specified to KSystemTimeZone.
void KTimeZoned::readZoneTab(QFile &f)
{
    // Parse the already open real or fake zone.tab.
    QRegExp lineSeparator("[ \t]");
    if (!mSource)
        mSource = new KSystemTimeZoneSource;
    mZones.clear();
    while (!f.atEnd())
    {
        // Read the next line, but limit its length to guard against crashing
        // due to a corrupt very large zone.tab (see KDE bug 224868).
        const QByteArray line = f.readLine(MAX_ZONE_TAB_LINE_LENGTH).trimmed();
        if (line.isEmpty() || line[0] == '#')
            continue;
        const QString lineStr = QString::fromLatin1(line.constData(), line.size());
        QStringList tokens = KStringHandler::perlSplit(lineSeparator, lineStr, 4);
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
    mLocalZone.clear();
    mLocalIdFile.clear();
    mLocalZoneDataFile.clear();

    // SOLUTION 1: DEFINITIVE.
    // First try the simplest solution of checking for well-formed TZ setting.
    const char *envtz = ::getenv("TZ");
    if (checkTZ(envtz)) {
        mSavedTZ = envtz;
        if (!mLocalZone.isEmpty()) {
            kDebug(1221) << "TZ: " << mLocalZone;
        }
    }

    if (mLocalZone.isEmpty() && !mZoneinfoDir.isEmpty()) {
        // SOLUTION 2: DEFINITIVE.
        // Try to follow any /etc/localtime symlink to a zoneinfo file.
        // SOLUTION 3: DEFINITIVE.
        // Try to match /etc/localtime against the list of zoneinfo files.
        matchZoneFile(QLatin1String("/etc/localtime"));
    }

    if (mLocalZone.isEmpty()) {
        // SOLUTION 4: DEFINITIVE.
        // BSD & Linux support: local time zone id in /etc/timezone.
        checkTimezone();
    }

    if (mLocalZone.isEmpty()) {
        // The local time zone is not defined by a file.
        // Watch for creation of /etc/localtime in case it gets created later.
        mLocalIdFile = QLatin1String("/etc/localtime");
    }

    if (mLocalZone.isEmpty() && !mZoneinfoDir.isEmpty()) {
        // SOLUTION 5: HEURISTIC.
        // None of the deterministic stuff above has worked: try a heuristic. We
        // try to find a pair of matching time zone abbreviations...that way, we'll
        // likely return a value in the user's own country.
        tzset();
        QByteArray tzname0(tzname[0]);   // store copies, because zone.parse() will change them
        QByteArray tzname1(tzname[1]);
        int bestOffset = INT_MAX;
        KSystemTimeZoneSource::startParseBlock();
        const KTimeZones::ZoneMap zmap = mZones.zones();
        for (KTimeZones::ZoneMap::ConstIterator it = zmap.constBegin(), end = zmap.constEnd();  it != end;  ++it) {
            KTimeZone zone = it.value();
            int candidateOffset = qAbs(zone.currentOffset(Qt::LocalTime));
            if (candidateOffset < bestOffset &&  zone.parse()) {
                QList<QByteArray> abbrs = zone.abbreviations();
                if (abbrs.contains(tzname0)  &&  abbrs.contains(tzname1)) {
                    kDebug(1221) << "local=" << zone.name();
                    mLocalZone = zone.name();
                    bestOffset = candidateOffset;
                    if (!bestOffset)
                        break;
                }
            }
        }
        KSystemTimeZoneSource::endParseBlock();
        if (!mLocalZone.isEmpty()) {
            kDebug(1221) << "tzname:" << mLocalZone;
        }
    }
    if (mLocalZone.isEmpty()) {
        // SOLUTION 6: FAILSAFE.
        mLocalZone = KTimeZone::utc().name();
        if (!mLocalZone.isEmpty()) {
            kDebug(1221) << "Failsafe:" << mLocalZone;
        }
    }

    // Finally, if the local zone identity has changed, store
    // the new one in the config file.
    updateLocalZone();
}

// Called when KDirWatch detects a change in zone.tab
void KTimeZoned::zonetab_Changed(const QString& path)
{
    kDebug(1221) << "zone.tab changed";
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
void KTimeZoned::localChanged()
{
    QByteArray envTz = qgetenv("TZ");
    QFileInfo info(mLocalIdFile);
    QDateTime currentstamp = info.lastModified();
    if (currentstamp == mLocalStamp && mSavedTZ == envTz)
        return;

    mLocalStamp = currentstamp;
    mSavedTZ = envTz;

    findLocalZone();
}

bool KTimeZoned::checkTZ(const char *envZone)
{
    // SOLUTION 1: DEFINITIVE.
    // First try the simplest solution of checking for well-formed TZ setting.
    if (envZone) {
        if (envZone[0] == '\0') {
            mLocalZone = KTimeZone::utc().name();
            mLocalIdFile.clear();
            mLocalZoneDataFile.clear();
            return true;
        }
        if (envZone[0] == ':') {
            // TZ specifies a file name, either relative to zoneinfo/ or absolute.
            QString TZfile = QFile::decodeName(envZone + 1);
            if (TZfile.startsWith(mZoneinfoDir)) {
                // It's an absolute file name in the zoneinfo directory.
                // Convert it to a file name relative to zoneinfo/.
                TZfile = TZfile.mid(mZoneinfoDir.length());
            }
            if (TZfile.startsWith(QLatin1Char('/'))) {
                // It's an absolute file name.
                if (matchZoneFile(TZfile))
                {
                    return true;
                }
            } else if (!TZfile.isEmpty()) {
                // It's a file name relative to zoneinfo/
                mLocalZone = TZfile;
                if (!mLocalZone.isEmpty())
                {
                    mLocalZoneDataFile = mZoneinfoDir + '/' + TZfile;
                    mLocalIdFile.clear();
                    return true;
                }
            }
        }
    }
    return false;
}

bool KTimeZoned::checkTimezone()
{
    // SOLUTION 2: DEFINITIVE.
    // BSD support.
    QFile f;
    f.setFileName(QLatin1String("/etc/timezone"));
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }
    // Read the first line of the file.
    QByteArray zoneName;
    if (!f.atEnd()) {
        zoneName = f.readLine().trimmed();
    }
    f.close();
    if (zoneName.isEmpty()) {
        return false;
    }
    if (!setLocalZone(QString::fromLatin1(zoneName.constData(), zoneName.size()))) {
        return false;
    }
    mLocalIdFile = f.fileName();
    kDebug(1221) << "/etc/timezone:" << mLocalZone;
    return true;
}

bool KTimeZoned::matchZoneFile(const QString &path)
{
    // SOLUTION 2: DEFINITIVE.
    // Try to follow any symlink to a zoneinfo file.
    // Get the path of the file which the symlink points to.
    QFile f;
    f.setFileName(path);
    QFileInfo fi(f);
    if (fi.isSymLink()) {
        // The file is a symlink.
        QString zoneInfoFileName = fi.canonicalFilePath();
        QFileInfo fiz(zoneInfoFileName);
        if (fiz.exists() && fiz.isReadable()) {
            if (zoneInfoFileName.startsWith(mZoneinfoDir)) {
                // We've got the zoneinfo file path.
                // The time zone name is the part of the path after the zoneinfo directory.
                // Note that some systems (e.g. Gentoo) have zones under zoneinfo which
                // are not in zone.tab, so don't validate against mZones.
                mLocalZone = zoneInfoFileName.mid(mZoneinfoDir.length() + 1);
                kDebug(1221) << "local=" << mLocalZone;
            } else {
                // It isn't a zoneinfo file or a copy thereof.
                // Use the absolute path as the time zone name.
                mLocalZone = f.fileName();
            }
            mLocalIdFile = f.fileName();
            mLocalZoneDataFile = zoneInfoFileName;
            kDebug(1221) << "symlink" << mLocalIdFile << ":" << mLocalZone;
            return true;
        }
    } else if (f.open(QIODevice::ReadOnly)) {
        // SOLUTION 3: DEFINITIVE.
        // Try to match the file against the list of zoneinfo files.

        KTimeZone local;
        QString zoneName;

        if (!mConfigLocalZone.isEmpty()) {
            // We know the local zone from last time.
            // Check whether the file still matches it.
            KTimeZone tzone = mZones.zone(mConfigLocalZone);
            if (tzone.isValid()) {
                zoneName = local.name();
            }
        }

        if (!local.isValid() && mHaveCountryCodes) {
            /* Look for time zones with the user's country code.
             * This has two advantages: 1) it shortens the search;
             * 2) it increases the chance of the correctly titled time zone
             * being found, since multiple time zones can have identical
             * definitions. For example, Europe/Guernsey is identical to
             * Europe/London, but the latter is more likely to be the right
             * zone name for a user with 'gb' country code.
             */
            QString country = KGlobal::locale()->country().toUpper();
            const KTimeZones::ZoneMap zmap = mZones.zones();
            for (KTimeZones::ZoneMap::ConstIterator zit = zmap.constBegin(), zend = zmap.constEnd();  zit != zend;  ++zit) {
                KTimeZone tzone = zit.value();
                if (tzone.countryCode() == country) {
                    if (tzone.isValid()) {
                        zoneName = tzone.name();
                        break;
                    }
                }
            }
        }

        bool success = false;
        if (local.isValid()) {
            // The file matches a zoneinfo file
            mLocalZone = zoneName;
            mLocalZoneDataFile = mZoneinfoDir + '/' + zoneName;
            success = true;
        } else {
            // The file doesn't match a zoneinfo file. If it's a TZfile, use it directly.
            // Read the file type identifier.
            char buff[4];
            f.reset();
            QDataStream str(&f);
            if (str.readRawData(buff, 4) == 4
            &&  buff[0] == 'T' && buff[1] == 'Z' && buff[2] == 'i' && buff[3] == 'f')
            {
                // Use its absolute path as the zone name.
                mLocalZone = f.fileName();
                mLocalZoneDataFile.clear();
                success = true;
            }
        }
        f.close();
        if (success) {
            mLocalIdFile = f.fileName();
            kDebug(1221) << "file" << mLocalIdFile << ":" << mLocalZone;
            return true;
        }
    }
    return false;
}

// Check whether the zone name is valid, either as a zone in zone.tab or
// as another file in the zoneinfo directory.
// If valid, set the local zone information.
bool KTimeZoned::setLocalZone(const QString &zoneName)
{
    KTimeZone local = mZones.zone(zoneName);
    const QString path = mZoneinfoDir + '/' + zoneName;
    if (!local.isValid()) {
        // It isn't a recognised zone in zone.tab.
        // Note that some systems (e.g. Gentoo) have zones under zoneinfo which
        // are not in zone.tab, so check if it points to another zone file.
        if (mZoneinfoDir.isEmpty())
            return false;
        QFileInfo fi(path);
        if (fi.isSymLink())
            fi.setFile(fi.canonicalFilePath());
        if (!fi.exists() || !fi.isReadable())
            return false;
    }
    mLocalZone = zoneName;
    mLocalZoneDataFile = mZoneinfoDir.isEmpty() ? QString() : path;
    return true;
}
