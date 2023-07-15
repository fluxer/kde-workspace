/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#include "location_timezone.h"

#include <KDebug>
#include <KSystemTimeZones>
#include <QLocale>

Timezone::Timezone(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setObjectName("location_timezone");
}

void Timezone::update()
{
    Plasma::DataEngine::Data outd;
    const KTimeZone ktimezone = KSystemTimeZones::local();
    const QString ktimezonename = ktimezone.name();
    const int slashindex = ktimezonename.lastIndexOf(QLatin1Char('/'));
    if (slashindex > 0) {
        const QString ktimezonecity = ktimezonename.mid(slashindex + 1, ktimezonename.size() - slashindex - 1);
        if (!ktimezonecity.isEmpty()) {
            const QLocale locale(ktimezone.countryCode());
            outd["accuracy"] = 60000;
            outd["latitude"] = ktimezone.latitude();
            outd["longitude"] = ktimezone.longitude();
            outd["country"] = QLocale::countryToString(locale.country());
            outd["country code"] = ktimezone.countryCode();
            outd["city"] = ktimezonecity;
            // qDebug() << Q_FUNC_INFO << outd;
        }
    }
    setData(outd);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(timezone, Timezone)

#include "moc_location_timezone.cpp"
