/*
 *   Copyright (C) 2007 Tobias Koenig <tokoe@kde.org>
 *   Copyright (C) 2008 Marco Martin <notmart@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU Library General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef FAVICONS_DATAENGINE_H
#define FAVICONS_DATAENGINE_H

#include <Plasma/DataEngine>

/**
 * This class provides favicons for websites
 *
 * the queries are just the url of websites we want to fetch an icon
 */
class FaviconsEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        FaviconsEngine(QObject* parent, const QVariantList& args);

    protected:
        bool sourceRequestEvent(const QString &identifier);

    protected Q_SLOTS:
        bool updateSourceEvent(const QString &identifier);
};

K_EXPORT_PLASMA_DATAENGINE(favicons, FaviconsEngine)

#endif // FAVICONS_DATAENGINE_H
