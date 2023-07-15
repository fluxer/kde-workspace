/*
 *   Copyright (C) 2009 Aaron Seigo <aseigo@kde.org>
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

#include "geolocationprovider.h"

#include <kdebug.h>

GeolocationProvider::GeolocationProvider(QObject *parent, const QVariantList &args)
    : QObject(parent),
      m_sharedData(nullptr),
      m_updating(false)
{
    Q_UNUSED(args);
}

void GeolocationProvider::init(Plasma::DataEngine::Data *data)
{
    m_sharedData = data;
}

void GeolocationProvider::requestUpdate()
{
    if (!m_updating) {
        m_updating = true;
        update();
    }
}

bool GeolocationProvider::isUpdating() const
{
    return m_updating;
}

void GeolocationProvider::setData(const Plasma::DataEngine::Data &data)
{
    m_updating = false;
    const QString providername = objectName();
    m_sharedData->insert(providername, data);
    // qDebug() << Q_FUNC_INFO << providername << m_updating << << data;
    emit updated();
}

void GeolocationProvider::update()
{
}

#include "moc_geolocationprovider.cpp"
