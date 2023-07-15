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
#include <KIO/StoredTransferJob>
#include <QJsonDocument>

static const QString s_geolocateurl = QString::fromLatin1("https://location.services.mozilla.com/v1/geolocate?key=b9255e08-838f-4d4e-b270-bec2cc89719b");
static const QString s_regionurl = QString::fromLatin1("https://location.services.mozilla.com/v1/country?key=b9255e08-838f-4d4e-b270-bec2cc89719b");

Mozilla::Mozilla(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setObjectName("location_mozilla");
}

void Mozilla::update()
{
    m_outdata.clear();
    kDebug() << "Fetching" << s_geolocateurl;
    KIO::StoredTransferJob *datajob = KIO::storedGet(
        KUrl(s_geolocateurl),
        KIO::NoReload, KIO::HideProgressInfo
    );
    datajob->setAutoDelete(false);
    connect(datajob, SIGNAL(result(KJob*)), this, SLOT(resultGeo(KJob*)));
}

void Mozilla::resultGeo(KJob* job)
{
    if (job->error() == KJob::NoError) {
        KIO::StoredTransferJob *datajob = qobject_cast<KIO::StoredTransferJob*>(job);
        const QJsonDocument jsondoc = QJsonDocument::fromJson(datajob->data());
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        const QVariantMap jsonlocationmap = jsonmap["location"].toMap();
        // for reference:
        // https://ichnaea.readthedocs.io/en/latest/api/geolocate.html
        m_outdata["accuracy"] = jsonmap["accuracy"];
        m_outdata["latitude"] = jsonlocationmap["lat"];
        m_outdata["longitude"] = jsonlocationmap["lng"];
        //qDebug() << Q_FUNC_INFO << jsonmap << jsonlocationmap;
    } else {
        kWarning() << "mozilla job error" << job->errorString();
    }
    job->deleteLater();

    kDebug() << "Fetching" << s_regionurl;
    KIO::StoredTransferJob *datajob = KIO::storedGet(
        KUrl(s_regionurl),
        KIO::NoReload, KIO::HideProgressInfo
    );
    datajob->setAutoDelete(false);
    connect(datajob, SIGNAL(result(KJob*)), this, SLOT(resultRegion(KJob*)));
}

void Mozilla::resultRegion(KJob* job)
{
    if (job->error() == KJob::NoError) {
        KIO::StoredTransferJob *datajob = qobject_cast<KIO::StoredTransferJob*>(job);
        const QJsonDocument jsondoc = QJsonDocument::fromJson(datajob->data());
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        // for reference:
        // https://ichnaea.readthedocs.io/en/latest/api/region.html
        m_outdata["country code"] = jsonmap["country_code"];
        m_outdata["country"] = jsonmap["country_name"];
        // qDebug() << Q_FUNC_INFO << jsonmap;
    } else {
        kWarning() << "mozilla job error" << job->errorString();
    }
    job->deleteLater();
    setData(m_outdata);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(mozilla, Mozilla)

#include "moc_location_mozilla.cpp"
