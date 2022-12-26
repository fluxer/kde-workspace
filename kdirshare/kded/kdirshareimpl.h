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

#include <QThread>
#include <khttp.h>
#include <kdnssd.h>

class KDirServer : public KHTTP
{
public:
    KDirServer(QObject *parent = nullptr);

    bool setDirectory(const QString &directory);

protected:
    void respond(
        const QByteArray &url,
        QByteArray *outdata, ushort *outhttpstatus, KHTTPHeaders *outheaders, QString *outfilepath
    ) final;

private:
    QString m_directory;
};


class KDirShareImpl : public QThread
{
    Q_OBJECT
public:
    KDirShareImpl(QObject *parent = nullptr);
    ~KDirShareImpl();

    QString serve(const QString &dirpath,
                  const quint16 portmin, const quint16 portmax,
                  const QString &username, const QString &password);
    QString directory() const;
    quint16 portMin() const;
    quint16 portMax() const;
    QString user() const;
    QString password() const;

protected:
    void run() final;

Q_SIGNALS:
    void unblock();
    void serveError(const QString &error);

private Q_SLOTS:
    void slotUnblock();
    void slotServeError(const QString &error);

private:
    QString m_directory;
    quint16 m_portmin;
    quint16 m_portmax;
    QString m_user;
    QString m_password;
    QString m_error;
    bool m_starting;
    KDirServer* m_kdirserver;
    KDNSSD m_kdnssd;
};

#endif // KDIRSHAREIMPL_H
