/*
 *   Copyright (C) 2009 Petri Damstén <damu@iki.fi>
 *                  - Original Implementation.
 *                 2009 Andrew Coles  <andrew.coles@yahoo.co.uk>
 *                  - Extension to iplocationtools engine.
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

#include "location_hostip.h"
#include <KDebug>
#include <KJob>
#include <KIO/Job>
#include <KIO/TransferJob>

class HostIP::Private : public QObject {

public:
    QByteArray payload;

    void populateDataEngineData(Plasma::DataEngine::Data & outd)
    {
        QString country, countryCode, city, latitude, longitude, ip;
        const QList<QByteArray> &bl = payload.split('\n');
        payload.clear();
        foreach (const QByteArray &b, bl) {
            const QList<QByteArray> &t = b.split(':');
            if (t.count() > 1) {
                const QByteArray k = t[0];
                const QByteArray v = t[1];
                if (k == "Latitude") {
                    latitude = v;
                } else if (k == "Longitude") {
                    longitude = v;
                } else if (k == "Country") {
                    QStringList cc = QString(v).split('(');
                    if (cc.count() > 1) {
                        country = cc[0].trimmed();
                        countryCode = cc[1].replace(')', "");
                    }
                } else if (k == "City") {
                    city = v;
                } else if (k == "IP") {
                    ip = v;
                }
            }
        }
        // ordering of first three to preserve backwards compatibility
        outd["accuracy"] = 40000;
        outd["country"] = country;
        outd["country code"] = countryCode;
        outd["city"] = city;
        outd["latitude"] = latitude;
        outd["longitude"] = longitude;
        outd["ip"] = ip;
        // qDebug() << Q_FUNC_INFO << outd;
    }
};

HostIP::HostIP(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args), d(new Private())
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

HostIP::~HostIP()
{
    delete d;
}

void HostIP::update()
{
    d->payload.clear();
    KIO::TransferJob *datajob = KIO::get(KUrl("https://api.hostip.info/get_html.php?position=true"),
                                         KIO::NoReload, KIO::HideProgressInfo);

    if (datajob) {
        kDebug() << "Fetching https://api.hostip.info/get_html.php?position=true";
        connect(datajob, SIGNAL(data(KIO::Job*,QByteArray)), this,
                SLOT(readData(KIO::Job*,QByteArray)));
        connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
    } else {
        kDebug() << "Could not create job";
    }
}

void HostIP::readData(KIO::Job* job, const QByteArray& data)
{
    Q_UNUSED(job)

    if (data.isEmpty()) {
        return;
    }
    d->payload.append(data);
}

void HostIP::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if(job && !job->error()) {
        d->populateDataEngineData(outd);
    } else {
        kDebug() << "error" << job->errorString();
    }

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(hostip, HostIP)

#include "moc_location_hostip.cpp"
