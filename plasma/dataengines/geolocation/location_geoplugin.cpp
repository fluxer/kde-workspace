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
#include <KIO/TransferJob>
#include <QJsonDocument>

class geoPlugin::Private
{
public:
    QByteArray payload;

    void populateDataEngineData(Plasma::DataEngine::Data & outd)
    {
        const QJsonDocument jsondoc = QJsonDocument::fromJson(payload);
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
    }
};

geoPlugin::geoPlugin(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args), d(new Private())
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

geoPlugin::~geoPlugin()
{
    delete d;
}

void geoPlugin::update()
{
    d->payload.clear();
    KIO::TransferJob *datajob = KIO::get(KUrl("http://www.geoplugin.net/json.gp"),
                                         KIO::NoReload, KIO::HideProgressInfo);

    if (datajob) {
        kDebug() << "Fetching http://www.geoplugin.net/json.gp";
        connect(datajob, SIGNAL(data(KIO::Job*,QByteArray)), this,
                SLOT(readData(KIO::Job*,QByteArray)));
        connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
    } else {
        kDebug() << "Could not create job";
    }
}

void geoPlugin::readData(KIO::Job* job, const QByteArray& data)
{
    Q_UNUSED(job)

    if (data.isEmpty()) {
        return;
    }
    d->payload.append(data);
}

void geoPlugin::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if(job && !job->error()) {
        d->populateDataEngineData(outd);
    } else {
        kDebug() << "error" << job->errorString();
    }

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(geoplugin, geoPlugin)

#include "moc_location_geoplugin.cpp"
