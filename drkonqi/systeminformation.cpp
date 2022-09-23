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

#include <sys/utsname.h>
#include <string.h>
#include <errno.h>

#include <QtCore/QFile>

#include <KDebug>
#include <kdeversion.h>

SystemInformation::SystemInformation(QObject * parent)
    : QObject(parent)
{
    // NOTE: the relative order is important here
    m_system = fetchOSDetailInformation();
    m_release = fetchOSReleaseInformation();
}

SystemInformation::~SystemInformation()
{
}

QString SystemInformation::fetchOSDetailInformation() const
{
    QString operatingSystem = "unspecified";
    struct utsname buf;
    if (::uname(&buf) == -1) {
        kDebug() << "call to uname failed" << ::strerror(errno);
    } else {
        operatingSystem = QString::fromLocal8Bit(buf.sysname) + ' '
            + QString::fromLocal8Bit(buf.release) + ' '
            + QString::fromLocal8Bit(buf.machine);
    }
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
    return distroInfos.value("PRETTY_NAME", "unspecified");
}

QString SystemInformation::system() const
{
    return m_system;
}

QString SystemInformation::release() const
{
    return m_release;
}

QString SystemInformation::kdeVersion() const
{
    return KDE::versionString();
}

QString SystemInformation::qtVersion() const
{
    return qVersion();
}
