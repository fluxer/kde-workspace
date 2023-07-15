/*
 *   Copyright (C) 2009 Petri Damsten <damu@iki.fi>
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

#include "geolocation.h"

#include <limits.h>

#include <KDebug>
#include <KServiceTypeTrader>

static const char SOURCE[] = "location";

Geolocation::Geolocation(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args),
    m_networkManager(nullptr)
{
    Q_UNUSED(args)
    setMinimumPollingInterval(500);
    m_networkManager = new KNetworkManager(this);
    connect(
        m_networkManager, SIGNAL(statusChanged(KNetworkManager::KNetworkStatus)),
        this, SLOT(networkStatusChanged(KNetworkManager::KNetworkStatus))
    );
}

void Geolocation::init()
{
    QVariantList args;
    const KService::List offers = KServiceTypeTrader::self()->query("Plasma/GeolocationProvider");
    foreach (const KService::Ptr service, offers) {
        QString error;
        GeolocationProvider *plugin = service->createInstance<GeolocationProvider>(nullptr, args, &error);
        if (plugin) {
            m_plugins << plugin;
            plugin->init(&m_data);
            connect(plugin, SIGNAL(updated()), this, SLOT(pluginUpdated()));
        } else {
            kWarning() << "failed to load GeolocationProvider" << error;
        }
    }
}

Geolocation::~Geolocation()
{
    qDeleteAll(m_plugins);
    m_plugins.clear();
}

QStringList Geolocation::sources() const
{
    return QStringList() << SOURCE;
}

void Geolocation::updatePlugins()
{
    kDebug() << "started update";
    foreach (GeolocationProvider *plugin, m_plugins) {
        plugin->requestUpdate();
    }
}

bool Geolocation::sourceRequestEvent(const QString &name)
{
    if (name == SOURCE) {
        m_data.clear();
        setData(SOURCE, Plasma::DataEngine::Data());
        updatePlugins();
        return true;
    }
    return false;
}

bool Geolocation::updateSourceEvent(const QString &name)
{
    if (name == SOURCE) {
        updatePlugins();
        return true;
    }
    return false;
}

void Geolocation::networkStatusChanged(const KNetworkManager::KNetworkStatus status)
{
    kDebug() << "network status changed";
    if (status == KNetworkManager::ConnectedStatus) {
        updatePlugins();
    }
}

void Geolocation::pluginUpdated()
{
    foreach (const GeolocationProvider *plugin, m_plugins) {
        if (plugin->isUpdating()) {
            return;
        }
    }
    kDebug() << "finished update";
    setData(SOURCE, m_data);
}

#include "moc_geolocation.cpp"
