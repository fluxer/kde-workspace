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
#include <KIO/StoredTransferJob>

HostIP::HostIP(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setUpdateTriggers(SourceEvent | NetworkConnected);
}

void HostIP::update()
{
    kDebug() << "Fetching https://api.hostip.info/get_html.php?position=true";
    KIO::StoredTransferJob *datajob = KIO::storedGet(
        KUrl("https://api.hostip.info/get_html.php?position=true"),
        KIO::NoReload, KIO::HideProgressInfo
    );
    datajob->setAutoDelete(false);
    connect(datajob, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
}

void HostIP::result(KJob* job)
{
    Plasma::DataEngine::Data outd;

    if (job->error() == KJob::NoError) {
        KIO::StoredTransferJob *datajob = qobject_cast<KIO::StoredTransferJob*>(job);

        QString country, countryCode, city, latitude, longitude, ip;
        const QList<QByteArray> &bl = datajob->data().split('\n');
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

        if (country.contains(QLatin1String("Unknown Country"))) {
            country = QString();
        }
        if (city.contains(QLatin1String("Unknown City"))) {
            city = QString();
        }
        if (country.isEmpty() && city.isEmpty()) {
            return;
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
    } else {
        kWarning() << "hostip job error" << job->errorString();
    }
    job->deleteLater();

    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(hostip, HostIP)

#include "moc_location_hostip.cpp"
