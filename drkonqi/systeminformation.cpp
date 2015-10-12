/*******************************************************************
* systeminformation.cpp
* Copyright 2009    Dario Andres Rodriguez <andresbajotierra@gmail.com>
* Copyright 2009    George Kiagiadakis <gkiagia@users.sourceforge.net>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************/

#include <config-drkonqi.h>

#include "systeminformation.h"

#ifdef HAVE_UNAME
# include <sys/utsname.h>
#endif

#include <QtCore/QFile>

#include <KStandardDirs>
#include <KProcess>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <kdeversion.h>

SystemInformation::SystemInformation(QObject * parent)
    : QObject(parent)
{
    // NOTE: the relative order is important here
    m_system = fetchOSDetailInformation();
    m_release = fetchOSReleaseInformation();

    KConfigGroup config(KGlobal::config(), "SystemInformation");
    m_compiledSources = config.readEntry("CompiledSources", false);
}

SystemInformation::~SystemInformation()
{
    KConfigGroup config(KGlobal::config(), "SystemInformation");
    config.writeEntry("CompiledSources", m_compiledSources);
    config.sync();
}

//this function maps the operating system to an OS value that is accepted by bugs.kde.org.
//if the values change on the server side, they need to be updated here as well.
QString SystemInformation::fetchOSBasicInformation() const
{
    //krazy:excludeall=cpp
    //Get the base OS string (bugzillaOS)
#if defined(Q_OS_LINUX)
    return QLatin1String("Linux");
#elif defined(Q_OS_FREEBSD)
    return QLatin1String("FreeBSD");
#elif defined(Q_OS_NETBSD)
    return QLatin1String("NetBSD");
#elif defined(Q_OS_OPENBSD)
    return QLatin1String("OpenBSD");
#elif defined(Q_OS_AIX)
    return QLatin1String("AIX");
#elif defined(Q_OS_HPUX)
    return QLatin1String("HP-UX");
#elif defined(Q_OS_IRIX)
    return QLatin1String("IRIX");
#elif defined(Q_OS_OSF)
    return QLatin1String("Tru64");
#elif defined(Q_OS_SOLARIS)
    return QLatin1String("Solaris");
#elif defined(Q_OS_CYGWIN)
    return QLatin1String("Cygwin");
#elif defined(Q_OS_WIN32)
    return QLatin1String("MS Windows");
#else
    return OS_UNSPECIFIED;
#endif

}

QString SystemInformation::fetchOSDetailInformation() const
{
    QString operatingSystem = "unspecified";
#ifdef HAVE_UNAME
    struct utsname buf;
    if (uname(&buf) == -1) {
        kDebug() << "call to uname failed" << perror;
    } else {
        operatingSystem = QString::fromLocal8Bit(buf.sysname) + ' '
            + QString::fromLocal8Bit(buf.release) + ' '
            + QString::fromLocal8Bit(buf.machine);
    }
#endif

    return operatingSystem;
}

QString SystemInformation::fetchOSReleaseInformation() const
{
    QFile data("/etc/os-release");
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QMap<QString,QString> distroInfos;

    QTextStream in(&data);
    while (!in.atEnd()) {
        const QString line = in.readLine();

        // its format is one simple NAME=VALUE per line
        // don't use QString.split() here since its value might contain '=''
        const int index = line.indexOf('=');
        if ( index != -1 ) {
            const QString key = line.left(index);
            QString value = line.mid(index+1);
            value = value.trimmed();
            if(value.startsWith("'") or value.startsWith("\"")) {
                value.remove(0, 1);
            }
            if(value.endsWith("'") or value.endsWith("\"")) {
                value.chop(1);
            }
            distroInfos.insert(key, value);
        }
    }

    // the PRETTY_NAME entry should be the most appropriate one,
    // but I could be wrong.
    const QString prettyName = distroInfos.value("PRETTY_NAME", "Linux");
    return prettyName;
}

QString SystemInformation::system() const
{
    return m_system;
}

QString SystemInformation::release() const
{
    return m_release;
}

bool SystemInformation::compiledSources() const
{
    return m_compiledSources;
}

void SystemInformation::setCompiledSources(bool compiled)
{
    m_compiledSources = compiled;
}

QString SystemInformation::kdeVersion() const
{
    return KDE::versionString();
}

QString SystemInformation::qtVersion() const
{
    return qVersion();
}
