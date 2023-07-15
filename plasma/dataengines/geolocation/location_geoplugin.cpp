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

#include "location_geoplugin.h"

#include <KDebug>
#include <KJob>
#include <KIO/StoredTransferJob>
#include <QJsonDocument>

geoPlugin::geoPlugin(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

void geoPlugin::update()
{
    kDebug() << "Fetching http://www.geoplugin.net/json.gp";
    KIO::StoredTransferJob *datajob = KIO::storedGet(
        KUrl("http://www.geoplugin.net/json.gp"),
        KIO::NoReload, KIO::HideProgressInfo
    );
    datajob->setAutoDelete(false);
    connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
}

void geoPlugin::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if (job->error() == KJob::NoError) {
        KIO::StoredTransferJob *datajob = qobject_cast<KIO::StoredTransferJob*>(job);
        const QJsonDocument jsondoc = QJsonDocument::fromJson(datajob->data());
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        // for reference:
        // http://www.geoplugin.com/quickstart
        outd["accuracy"] = 50000;
        outd["country"] = jsonmap["geoplugin_countryName"];;
        outd["country code"] = jsonmap["geoplugin_countryCode"];
        outd["city"] = jsonmap["geoplugin_city"];
        outd["latitude"] = jsonmap["geoplugin_latitude"];
        outd["longitude"] = jsonmap["geoplugin_longitude"];
        outd["ip"] = jsonmap["geoplugin_request"];
        // qDebug() << Q_FUNC_INFO << outd;
    } else {
        kWarning() << "geoplugin job error" << job->errorString();
    }
    job->deleteLater();

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(geoplugin, geoPlugin)

#include "moc_location_geoplugin.cpp"
