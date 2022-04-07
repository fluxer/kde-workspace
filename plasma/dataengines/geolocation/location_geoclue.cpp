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

#define QT_NO_KEYWORDS

#include "location_geoclue.h"

#include <KDebug>

#include <libgeoclue-2.0/gclue-simple.h>


GeoClue::GeoClue(QObject* parent, const QVariantList& args)
    : GeolocationProvider(parent, args)
{
    setUpdateTriggers(GeolocationProvider::SourceEvent | GeolocationProvider::NetworkConnected);
}

GeoClue::~GeoClue()
{
}

void GeoClue::update()
{
    Plasma::DataEngine::Data enginedata;
    setData(enginedata);

    GError *gliberror = NULL;
    GClueSimple* gcluesimple = gclue_simple_new_sync("location_geoclue", GCLUE_ACCURACY_LEVEL_CITY, NULL, &gliberror);
    if (!gcluesimple) {
        kWarning() << "Null GClueSimple pointer";
        return;
    } else if (gliberror != NULL) {
        kWarning() << gliberror->message;
        return;
    }

    GClueLocation* gcluelocation = gclue_simple_get_location(gcluesimple);
    if (!gcluelocation) {
        kWarning() << "Null GClueLocation pointer";
        return;
    }

    // qDebug() << Q_FUNC_INFO << gclue_location_get_description(gcluelocation);
    // qDebug() << Q_FUNC_INFO << gclue_location_get_accuracy(gcluelocation);
    // qDebug() << Q_FUNC_INFO << gclue_location_get_latitude(gcluelocation)
    // qDebug() << Q_FUNC_INFO << gclue_location_get_longitude(gcluelocation);
    enginedata["accuracy"] = qRound(qreal(gclue_location_get_accuracy(gcluelocation)));
    enginedata["latitude"] = qreal(gclue_location_get_latitude(gcluelocation));
    enginedata["longitude"] = qreal(gclue_location_get_longitude(gcluelocation));
    setData(enginedata);
}

K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(geoclue, GeoClue)

#include "moc_location_geoclue.cpp"
