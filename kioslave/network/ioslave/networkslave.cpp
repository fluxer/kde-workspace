/*
    This file is part of the network kioslave, part of the KDE project.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkslave.h"

#include <KMimeType>
#include <KDebug>

#include <sys/stat.h>

static QString urlForService(const KDNSSDService &kdnssdservice)
{
    // for compatibility since there is no KIO slave to open rfb protocols
    if (kdnssdservice.url.startsWith(QLatin1String("rfb://"))) {
        QString result = kdnssdservice.url;
        result = result.replace(QLatin1String("rfb://"), QLatin1String("vnc://"));
        return result;
    }
    return kdnssdservice.url;
}

static QString mimeForService(const KDNSSDService &kdnssdservice)
{
    return QString::fromLatin1("inode/vnd.kde.service.%1").arg(
        KUrl(kdnssdservice.url).protocol()
    );
}

static QString iconForService(const KDNSSDService &kdnssdservice)
{
    return KMimeType::mimeType(mimeForService(kdnssdservice))->iconName();
}

NetworkSlave::NetworkSlave(const QByteArray &name, const QByteArray &poolSocket, const QByteArray &programSocket)
    : SlaveBase(name, poolSocket, programSocket)
{
}

NetworkSlave::~NetworkSlave()
{
}

void NetworkSlave::mimetype(const KUrl &url)
{
    m_kdnssd.startBrowse();
    foreach (const KDNSSDService &kdnssdservice, m_kdnssd.services()) {
        // qDebug() << Q_FUNC_INFO << kdnssdservice.url << url.prettyUrl();
        if (kdnssdservice.url == url.prettyUrl()) {
            mimeType(mimeForService(kdnssdservice));
            finished();
            return;
        }
    }
    error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
}

void NetworkSlave::stat(const KUrl &url)
{
    m_kdnssd.startBrowse();
    foreach (const KDNSSDService &kdnssdservice, m_kdnssd.services()) {
        // qDebug() << Q_FUNC_INFO << kdnssdservice.url << url.prettyUrl();
        if (kdnssdservice.url == url.prettyUrl()) {
            KIO::UDSEntry kioudsentry;
            kioudsentry.insert(KIO::UDSEntry::UDS_NAME, kdnssdservice.name);
            kioudsentry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFLNK);
            kioudsentry.insert(KIO::UDSEntry::UDS_ACCESS, S_IRWXU | S_IRWXG | S_IRWXO);
            kioudsentry.insert(KIO::UDSEntry::UDS_ICON_NAME, iconForService(kdnssdservice));
            kioudsentry.insert(KIO::UDSEntry::UDS_MIME_TYPE, mimeForService(kdnssdservice));
            kioudsentry.insert(KIO::UDSEntry::UDS_TARGET_URL, urlForService(kdnssdservice));
            statEntry(kioudsentry);
            finished();
            return;
        }
    }
    error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
}

void NetworkSlave::listDir(const KUrl &url)
{
    m_kdnssd.startBrowse();
    KIO::UDSEntry kioudsentry;
    foreach (const KDNSSDService &kdnssdservice, m_kdnssd.services()) {
        kioudsentry.clear();
        kioudsentry.insert(KIO::UDSEntry::UDS_NAME, kdnssdservice.name);
        kioudsentry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFLNK);
        kioudsentry.insert(KIO::UDSEntry::UDS_ACCESS, S_IRWXU | S_IRWXG | S_IRWXO);
        kioudsentry.insert(KIO::UDSEntry::UDS_ICON_NAME, iconForService(kdnssdservice));
        kioudsentry.insert(KIO::UDSEntry::UDS_MIME_TYPE, mimeForService(kdnssdservice));
        kioudsentry.insert(KIO::UDSEntry::UDS_TARGET_URL, urlForService(kdnssdservice));
        listEntry(kioudsentry, false);
    }
    listEntry(kioudsentry, true);
    finished();
}
