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

#ifndef KDIRSHAREIMPL_H
#define KDIRSHAREIMPL_H

#include <khttp.h>
#include <kdnssd.h>

static const quint16 s_kdirshareportmin = 1000;
static const quint16 s_kdirshareportmax = 32000;

class KDirShareImpl : public KHTTP
{
    Q_OBJECT
public:
    KDirShareImpl(QObject *parent = nullptr);
    ~KDirShareImpl();

    QString directory() const;
    bool setDirectory(const QString &dirpath);
    bool serve(const QHostAddress &address, const quint16 portmin, const quint16 portmax);
    bool publish();

    quint16 portMin() const;
    quint16 portMax() const;

protected:
    void respond(const QByteArray &url, QByteArray *outdata, ushort *outhttpstatus, KHTTPHeaders *outheaders) final;

private:
    QString m_directory;
    quint16 m_port;
    quint16 m_portmin;
    quint16 m_portmax;
    KDNSSD m_kdnssd;
};

#endif // KDIRSHAREIMPL_H
