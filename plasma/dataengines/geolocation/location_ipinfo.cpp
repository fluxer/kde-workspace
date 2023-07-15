/*  This file is part of the KDE project
    Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>

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

#include "location_ipinfo.h"

#include <KDebug>
#include <KJob>
#include <KIO/StoredTransferJob>
#include <QJsonDocument>
#include <QLocale>

IPinfo::IPinfo(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setObjectName("location_ipinfo");
}

void IPinfo::update()
{
    kDebug() << "Fetching https://ipinfo.io/json";
    KIO::StoredTransferJob *datajob = KIO::storedGet(
        KUrl("https://ipinfo.io/json"),
        KIO::NoReload, KIO::HideProgressInfo
    );
    datajob->setAutoDelete(false);
    connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
}

void IPinfo::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if (job->error() == KJob::NoError) {
        KIO::StoredTransferJob *datajob = qobject_cast<KIO::StoredTransferJob*>(job);
        const QJsonDocument jsondoc = QJsonDocument::fromJson(datajob->data());
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        const QStringList location = jsonmap["loc"].toString().split(QLatin1Char(','));
        const QString latitude = (location.size() == 2 ? location.at(0) : QString());
        const QString longitude = (location.size() == 2 ? location.at(1) : QString());
        const QLocale locale(jsonmap["country"].toString());
        // for reference:
        // https://ipinfo.io/developers
        outd["accuracy"] = 50000;
        outd["country"] = QLocale::countryToString(locale.country());
        outd["country code"] = jsonmap["country"];
        outd["city"] = jsonmap["city"];
        outd["latitude"] = latitude;
        outd["longitude"] = longitude;
        outd["ip"] = jsonmap["ip"];
        // qDebug() << Q_FUNC_INFO << outd;
    } else {
        kWarning() << "ipinfo job error" << job->errorString();
    }
    job->deleteLater();

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(ipinfo, IPinfo)

#include "moc_location_ipinfo.cpp"
