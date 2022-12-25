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
#include "kdirshare.h"

#include <QDir>
#include <QBuffer>
#include <QPixmap>
#include <QHostInfo>
#include <kicon.h>
#include <klocale.h>
#include <kmimetype.h>
#include <krandom.h>
#include <kdebug.h>

static const QDir::SortFlags s_dirsortflags = (QDir::Name | QDir::DirsFirst);
static const QByteArray s_data404("<html>404 Not Found</html>");
static const QByteArray s_data500("<html>500 Internal Server Error</html>");
// AVAHI_LABEL_MAX - 3 (for dots) - 1 (for null terminator)
static const int s_sharenamelimit = 60;

static quint16 getPort(const quint16 portmin, const quint16 portmax)
{
    if (portmin == portmax) {
        return portmax;
    }
    quint16 portnumber = 0;
    while (portnumber < portmin) {
        portnumber = quint16(KRandom::randomMax(portmax));
        Q_ASSERT(portnumber <= portmax);
    }
    return portnumber;
}

static QString getFileMIME(const QString &filepath)
{
    const KMimeType::Ptr kmimetypeptr = KMimeType::findByUrl(
        KUrl(filepath),
        mode_t(0), true
    );
    if (!kmimetypeptr.isNull()) {
        return kmimetypeptr->name();
    }
    return QString::fromLatin1("application/octet-stream");
}

static QString getTitle(const QString &dirpath)
{
    const QString sharename = QDir(QDir(dirpath).absolutePath()).dirName();
    QString title = i18n("KDirShare@%1 (%2)", QHostInfo::localHostName(), sharename);
    if (title.size() > s_sharenamelimit) {
        title = title.left(s_sharenamelimit);
        title.append(QLatin1String("..."));
    }
    return title;
}

static QByteArray contentForDirectory(const QString &path, const QString &basedir)
{
    const QString pathtitle = getTitle(path);

    QByteArray data;
    data.append("<html>\n");
    data.append("  <body>\n");
    data.append("    <title>");
    data.append(pathtitle.toUtf8());
    data.append("</title>\n");
    data.append("    <table>\n");
    data.append("      <tr>\n");
    data.append("        <th></th>\n"); // icon
    data.append("        <th>Filename</th>\n");
    data.append("        <th>MIME</th>\n");
    data.append("        <th>Size</th>\n");
    data.append("      </tr>\n");
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

        data.append("      <tr>\n");

        const bool isdotdot = (fileinfo.fileName() == QLatin1String(".."));
        if (isdotdot) {
            const QString fileicon = QString::fromLatin1("<img src=\"/kdirshare_icons/go-previous\" width=\"20\" height=\"20\">");
            data.append("        <td>");
            data.append(fileicon.toUtf8());
            data.append("</td>\n");
        } else {
            const QString fileicon = QString::fromLatin1("<img src=\"/kdirshare_icons/%1\" width=\"20\" height=\"20\">").arg(KMimeType::iconNameForUrl(KUrl(fullpath)));
            data.append("        <td>");
            data.append(fileicon.toUtf8());
            data.append("</td>\n");
        }

        // qDebug() << Q_FUNC_INFO << fullpath << basedir << cleanpath;
        data.append("        <td><a href=\"");
        data.append(cleanpath.toUtf8());
        data.append("\">");
        data.append(fileinfo.fileName().toUtf8());
        data.append("</a><br></td>\n");

        data.append("        <td>");
        if (!isdotdot) {
            const QString filemime = getFileMIME(fullpath);
            data.append(filemime.toUtf8());
        }
        data.append("</td>\n");

        data.append("        <td>");
        if (fileinfo.isFile()) {
            const QString filesize = KGlobal::locale()->formatByteSize(fileinfo.size(), 1);
            data.append(filesize.toUtf8());
        }
        data.append("</td>\n");

        data.append("      </tr>\n");
    }
    data.append("    </table>\n");
    data.append("  </body>\n");
    data.append("</html>");
    return data;
}

KDirShareImpl::KDirShareImpl(QObject *parent)
    : KHTTP(parent),
    m_directory(QDir::currentPath()),
    m_portmin(s_kdirshareportmin),
    m_portmax(s_kdirshareportmax)
{
}

KDirShareImpl::~KDirShareImpl()
{
    m_kdnssd.unpublishService();
    stop();
}

QString KDirShareImpl::serve(const QString &dirpath,
                             const quint16 portmin, const quint16 portmax,
                             const QString &username, const QString &password)
{
    // qDebug() << Q_FUNC_INFO << dirpath << portmin << portmax << username << password;
    const quint16 port = getPort(portmin, portmax);
    m_directory = dirpath;
    m_portmin = portmin;
    m_portmax = portmax;
    m_user = username;
    m_password = password;
    m_error.clear();
    if (!QDir(m_directory).exists()) {
        m_error = i18n("Directory does not exist: %1", m_directory);
        return m_error;
    }
    if (!m_user.isEmpty() && !m_password.isEmpty()) {
        if (!setAuthenticate(m_user.toUtf8(), m_password.toUtf8())) {
            m_error = i18n("Could not set authentication: %1", errorString());
            return m_error;
        }
    }
    if (!start(QHostAddress(QHostAddress::Any), port)) {
        m_error = i18n("Could not serve: %1", errorString());
        return m_error;
    }
    if (!m_kdnssd.publishService("_http._tcp", port, getTitle(m_directory))) {
        stop();
        m_error = i18n("Could not publish service: %1", m_kdnssd.errorString());
        return m_error;
    }
    return m_error;
}
QString KDirShareImpl::directory() const
{
    return m_directory;
}

quint16 KDirShareImpl::portMin() const
{
    return m_portmin;
}

quint16 KDirShareImpl::portMax() const
{
    return m_portmax;
}

QString KDirShareImpl::user() const
{
    return m_user;
}

QString KDirShareImpl::password() const
{
    return m_password;
}

// for reference:
// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
void KDirShareImpl::respond(const QByteArray &url, QByteArray *outdata,
                            ushort *outhttpstatus, KHTTPHeaders *outheaders,
                            QString *outfilepath)
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
        const QString filemime = getFileMIME(pathinfo.filePath());
        *outhttpstatus = 200;
        outheaders->insert("Content-Type", QString::fromLatin1("%1; charset=UTF-8").arg(filemime).toAscii());
        outfilepath->append(pathinfo.filePath());
    } else {
        outdata->append(s_data404);
        *outhttpstatus = 404;
        outheaders->insert("Content-Type", "text/html; charset=UTF-8");
    }
}

#include "moc_kdirshareimpl.cpp"
