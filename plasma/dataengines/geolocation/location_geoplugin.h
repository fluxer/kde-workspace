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

#ifndef GEOPLUGIN_H
#define GEOPLUGIN_H

#include "geolocationprovider.h"

#include <KIO/Job>

class geoPlugin : public GeolocationProvider
{
    Q_OBJECT
public:
    explicit geoPlugin(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~geoPlugin();

    virtual void update();

protected slots:
    void readData(KIO::Job *, const QByteArray& data);
    void result(KJob* job);

private:
    class Private;
    Private *const d;
};

#endif // GEOPLUGIN_H