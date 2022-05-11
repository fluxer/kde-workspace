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

#include "location_mozilla.h"

#include <KDebug>
#include <KJob>
#include <KIO/TransferJob>
#include <QJsonDocument>

static const QString s_geolocateurl = QString::fromLatin1("https://location.services.mozilla.com/v1/geolocate?key=b9255e08-838f-4d4e-b270-bec2cc89719b");
static const QString s_regionurl = QString::fromLatin1("https://location.services.mozilla.com/v1/country?key=b9255e08-838f-4d4e-b270-bec2cc89719b");

class Mozilla::Private
{
public:
    QByteArray payload;

    void populateGeoDataEngineData()
    {
        const QJsonDocument jsondoc = QJsonDocument::fromJson(payload);
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        const QVariantMap jsonlocationmap = jsonmap["location"].toMap();
        // for reference:
        // https://ichnaea.readthedocs.io/en/latest/api/geolocate.html
        outd["accuracy"] = jsonmap["accuracy"];
        outd["latitude"] = jsonlocationmap["lat"];
        outd["longitude"] = jsonlocationmap["lng"];
        //qDebug() << Q_FUNC_INFO << jsonmap << jsonlocationmap;
    }

    void populateRegionDataEngineData()
    {
        const QJsonDocument jsondoc = QJsonDocument::fromJson(payload);
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        // for reference:
        // https://ichnaea.readthedocs.io/en/latest/api/region.html
        outd["country code"] = jsonmap["country_code"];
        outd["country"] = jsonmap["country_name"];
        // qDebug() << Q_FUNC_INFO << jsonmap;
    }

    Plasma::DataEngine::Data outd;
};

Mozilla::Mozilla(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args), d(new Private())
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

Mozilla::~Mozilla()
{
    delete d;
}

void Mozilla::update()
{
    d->outd.clear();
    d->payload.clear();
    KIO::TransferJob *datajob = KIO::get(KUrl(s_geolocateurl), KIO::NoReload, KIO::HideProgressInfo);
    if (datajob) {
        kDebug() << "Fetching" << s_geolocateurl;
        connect(datajob, SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT(readData(KIO::Job*,QByteArray)));
        connect(datajob, SIGNAL(result(KJob*)), this, SLOT(resultGeo(KJob*)));
    } else {
        kDebug() << "Could not create geo job";
    }
}

void Mozilla::readData(KIO::Job* job, const QByteArray &data)
{
    Q_UNUSED(job)

    if (data.isEmpty()) {
        return;
    }
    d->payload.append(data);
}

void Mozilla::resultGeo(KJob* job)
{
    if (job && !job->error()) {
        d->populateGeoDataEngineData();
    } else {
        kDebug() << "error" << job->errorString();
    }

    d->payload.clear();
    KIO::TransferJob *datajob = KIO::get(KUrl(s_regionurl), KIO::NoReload, KIO::HideProgressInfo);
    if (datajob) {
        kDebug() << "Fetching" << s_regionurl;
        connect(datajob, SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT(readData(KIO::Job*,QByteArray)));
        connect(datajob, SIGNAL(result(KJob*)), this, SLOT(resultRegion(KJob*)));
    } else {
        kDebug() << "Could not create region job";
        setData(d->outd);
        // qDebug() << Q_FUNC_INFO << d->outd;
    }
}

void Mozilla::resultRegion(KJob* job)
{
    if (job && !job->error()) {
        d->populateRegionDataEngineData();
    } else {
        kDebug() << "error" << job->errorString();
    }
    setData(d->outd);
    // qDebug() << Q_FUNC_INFO << d->outd;
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(mozilla, Mozilla)

#include "moc_location_mozilla.cpp"
