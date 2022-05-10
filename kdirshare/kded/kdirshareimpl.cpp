/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kdirshareimpl.h"

#include <QDir>
#include <QBuffer>
#include <QPixmap>
#include <QHostInfo>
#include <kicon.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kdebug.h>

static const QDir::SortFlags s_dirsortflags = (QDir::Name | QDir::DirsFirst);
static const QByteArray s_data404("<html>404 Not Found</html>");
static const QByteArray s_data500("<html>500 Internal Server Error</html>");

static quint16 getPort(const quint16 portmin, const quint16 portmax)
{
    if (portmin == portmax) {
        return portmax;
    }
    quint16 portnumber = 0;
    while (portnumber < portmin || portnumber > portmax) {
        portnumber = quint16(qrand());
    }
    return portnumber;
}

static QString getShareName(const QString &dirpath)
{
    const QString absolutedirpath = QDir(dirpath).absolutePath();
    QString basedirname = QDir(absolutedirpath).dirName();
    // TODO: figure out what the Avahi limit is
    basedirname = basedirname.left(40);
    // qDebug() << Q_FUNC_INFO << basedirname;
    return basedirname;
}

static QByteArray contentForDirectory(const QString &path, const QString &basedir)
{
    QByteArray data;
    data.append("<html>");
    data.append("<table>");
    data.append("  <tr>");
    data.append("    <th></th>"); // icon
    data.append("    <th>Filename</th>");
    data.append("    <th>MIME</th>");
    data.append("    <th>Size</th>");
    data.append("  </tr>");
    QDir::Filters dirfilters = (QDir::Files | QDir::AllDirs | QDir::NoDot);
    if (QDir::cleanPath(path) == QDir::cleanPath(basedir)) {
        dirfilters = (QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
    }
    QDir dir(path);
    foreach (const QFileInfo &fileinfo, dir.entryInfoList(dirfilters, s_dirsortflags)) {
        const QString fullpath = path + QLatin1Char('/') + fileinfo.fileName();
        // chromium does weird stuff if the link starts with two slashes - removes, the host and
        // port part of the link (or rather does not prepend them) and converts the first directory
        // to lower-case
        const QString cleanpath = QDir::cleanPath(fullpath.mid(basedir.size()));

        data.append("  <tr>");

        const bool isdotdot = (fileinfo.fileName() == QLatin1String(".."));
        if (isdotdot) {
            const QString fileicon = QString::fromLatin1("<img src=\"/kdirshare_icons/go-previous\" width=\"20\" height=\"20\">");
            data.append("<td>");
            data.append(fileicon.toAscii());
            data.append("</td>");
        } else {
            const QString fileicon = QString::fromLatin1("<img src=\"/kdirshare_icons/%1\" width=\"20\" height=\"20\">").arg(KMimeType::iconNameForUrl(KUrl(fullpath)));
            data.append("<td>");
            data.append(fileicon.toAscii());
            data.append("</td>");
        }

        // qDebug() << Q_FUNC_INFO << fullpath << basedir << cleanpath;
        data.append("<td><a href=\"");
        data.append(cleanpath.toLocal8Bit());
        data.append("\">");
        data.append(fileinfo.fileName().toLocal8Bit());
        data.append("</a><br></td>");

        data.append("<td>");
        if (!isdotdot) {
            const QString filemime = KMimeType::findByPath(fullpath)->name();
            data.append(filemime.toAscii());
        }
        data.append("</td>");

        data.append("<td>");
        if (fileinfo.isFile()) {
            const QString filesize = KGlobal::locale()->formatByteSize(fileinfo.size(), 1);
            data.append(filesize.toAscii());
        }
        data.append("</td>");

        data.append("  </tr>");
    }
    data.append("</table>");
    data.append("</html>");
    return data;
}

KDirShareImpl::KDirShareImpl(QObject *parent)
    : KHTTP(parent),
    m_directory(QDir::currentPath()),
    m_port(0),
    m_portmin(s_kdirshareportmin),
    m_portmax(s_kdirshareportmax)
{
}

KDirShareImpl::~KDirShareImpl()
{
    m_kdnssd.unpublishService();
    stop();
}

QString KDirShareImpl::directory() const
{
    return m_directory;
}

bool KDirShareImpl::setDirectory(const QString &dirpath)
{
    if (!QDir(dirpath).exists()) {
        return false;
    }
    m_directory = dirpath;
    return true;
}

bool KDirShareImpl::serve(const QHostAddress &address, const quint16 portmin, const quint16 portmax)
{
    m_port = getPort(portmin, portmax);
    m_portmin = portmin;
    m_portmax = portmax;
    return start(address, m_port);
}

bool KDirShareImpl::publish()
{
    return m_kdnssd.publishService(
        "_http._tcp", m_port,
        i18n("KDirShare@%1 (%2)", QHostInfo::localHostName(), getShareName(m_directory))
    );
}

quint16 KDirShareImpl::portMin() const
{
    return m_portmin;
}

quint16 KDirShareImpl::portMax() const
{
    return m_portmax;
}

QString KDirShareImpl::publishError() const
{
    return m_kdnssd.errorString();
}

// for reference:
// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
void KDirShareImpl::respond(const QByteArray &url, QByteArray *outdata, ushort *outhttpstatus, KHTTPHeaders *outheaders)
{
    // qDebug() << Q_FUNC_INFO << url;

    outheaders->insert("Server", "KDirShare");

    const QString normalizedpath = QUrl::fromPercentEncoding(url);
    QFileInfo pathinfo(m_directory + QLatin1Char('/') + normalizedpath);
    // qDebug() << Q_FUNC_INFO << normalizedpath << pathinfo.filePath();
    if (normalizedpath.startsWith(QLatin1String("/kdirshare_icons/"))) {
        const QPixmap iconpixmap = KIcon(normalizedpath.mid(17)).pixmap(20);
        QBuffer iconbuffer;
        iconbuffer.open(QBuffer::WriteOnly);
        if (!iconpixmap.save(&iconbuffer, "PNG")) {
            kWarning() << "Could not save image";
            outdata->append(s_data500);
            *outhttpstatus = 500;
            outheaders->insert("Content-Type", "text/html; charset=UTF-8");
        } else {
            outdata->append(iconbuffer.data());
            *outhttpstatus = 200;
            outheaders->insert("Content-Type", "image/png");
        }
    } else if (pathinfo.isDir()) {
        *outhttpstatus = 200;
        outheaders->insert("Content-Type", "text/html; charset=UTF-8");
        outdata->append(contentForDirectory(pathinfo.filePath(), m_directory));
    } else if (pathinfo.isFile()) {
        QFile pathfile(pathinfo.filePath());
        if (!pathfile.open(QFile::ReadOnly)) {
            kWarning() << "Could not open" << pathinfo.filePath() << pathfile.errorString();
            outdata->append(s_data500);
            *outhttpstatus = 500;
            outheaders->insert("Content-Type", "text/html; charset=UTF-8");
        } else {
            const QString filemime = KMimeType::findByPath(pathinfo.filePath())->name();
            *outhttpstatus = 200;
            outheaders->insert("Content-Type", QString::fromLatin1("%1; charset=UTF-8").arg(filemime).toAscii());
            outdata->append(pathfile.readAll());
        };
    } else {
        outdata->append(s_data404);
        *outhttpstatus = 404;
        outheaders->insert("Content-Type", "text/html; charset=UTF-8");
    }
}

#include "moc_kdirshareimpl.cpp"
