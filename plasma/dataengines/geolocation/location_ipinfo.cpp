/*
 *   Copyright (C) 2009 Petri Damstén <damu@iki.fi>
 *                  - Original Implementation.
 *                 2009 Andrew Coles  <andrew.coles@yahoo.co.uk>
 *                  - Extension to iplocationtools engine.
 *                 2021 Ivailo Monev <xakepa10@gmail.com>
 *                  - Additional IP provider for geolocation engine.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "location_ipinfo.h"
#include <KDebug>
#include <KJob>
#include <KIO/Job>
#include <KIO/TransferJob>
#include <QJsonDocument>
#include <QLocale>

class IPinfo::Private : public QObject {

public:
    QByteArray payload;

    void populateDataEngineData(Plasma::DataEngine::Data & outd)
    {
        const QJsonDocument jsondoc = QJsonDocument::fromJson(payload);
        const QVariantMap jsonmap = jsondoc.toVariant().toMap();
        const QStringList location = jsonmap["loc"].toString().split(QLatin1Char(','));
        // TODO: which is which?
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
    }
};

IPinfo::IPinfo(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args), d(new Private())
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

IPinfo::~IPinfo()
{
    delete d;
}

void IPinfo::update()
{
    d->payload.clear();
    KIO::TransferJob *datajob = KIO::get(KUrl("https://ipinfo.io/json"),
                                         KIO::NoReload, KIO::HideProgressInfo);

    if (datajob) {
        kDebug() << "Fetching https://ipinfo.io/json";
        connect(datajob, SIGNAL(data(KIO::Job*,QByteArray)), this,
                SLOT(readData(KIO::Job*,QByteArray)));
        connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
    } else {
        kDebug() << "Could not create job";
    }
}

void IPinfo::readData(KIO::Job* job, const QByteArray& data)
{
    Q_UNUSED(job)

    if (data.isEmpty()) {
        return;
    }
    d->payload.append(data);
}

void IPinfo::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if(job && !job->error()) {
        d->populateDataEngineData(outd);
    } else {
        kDebug() << "error" << job->errorString();
    }

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(ipinfo, IPinfo)

#include "moc_location_ipinfo.cpp"
