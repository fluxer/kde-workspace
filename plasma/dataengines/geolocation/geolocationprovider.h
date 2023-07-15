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

#ifndef GEOLOCATIONPROVIDER_H
#define GEOLOCATIONPROVIDER_H

#include <QtCore/QHash>
#include <QtCore/QObject>

#include <Plasma/DataEngine>

#include "geolocation_export.h"

class GEOLOCATION_EXPORT GeolocationProvider : public QObject
{
    Q_OBJECT
public:
    GeolocationProvider(QObject *parent = 0, const QVariantList &args = QVariantList());
    void init(Plasma::DataEngine::Data *data);

    void requestUpdate();
    bool isUpdating() const;

Q_SIGNALS:
    void updated();

protected:
    virtual void update();

    void setData(const Plasma::DataEngine::Data &data);

private:
    Plasma::DataEngine::Data *m_sharedData;
    bool m_updating;
};

#define K_EXPORT_PLASMA_GEOLOCATIONPROVIDER(libname, classname) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN(factory("plasma_GeolocationProvider_" #libname))

#endif

