/*
 *   Copyright (C) 2009 Petri Damstén <damu@iki.fi>
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

#ifndef GEOLOCATION_DATAENGINE_H
#define GEOLOCATION_DATAENGINE_H

#include <QTimer>

#include <Plasma/DataEngine>
#include <KNetworkManager>

#include "geolocationprovider.h"

class GeolocationProvider;

class Geolocation : public Plasma::DataEngine
{
    Q_OBJECT
public:
    Geolocation(QObject* parent, const QVariantList& args);
    virtual ~Geolocation();

    void init() final;
    QStringList sources() const final;

protected:
    bool sourceRequestEvent(const QString &name) final;
    bool updateSourceEvent(const QString &name) final;

private slots:
    void networkStatusChanged(const KNetworkManager::KNetworkStatus status);
    void pluginUpdated();

private:
    void updatePlugins();

    Plasma::DataEngine::Data m_data;
    QList<GeolocationProvider *> m_plugins;
    KNetworkManager *m_networkManager;
};

K_EXPORT_PLASMA_DATAENGINE(geolocation, Geolocation)

#endif

