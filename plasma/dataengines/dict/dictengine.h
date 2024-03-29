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

#ifndef DICTENGINE_H
#define DICTENGINE_H

#include <Plasma/DataEngine>
#include <KJob>
#include <KIO/Job>

/**
 * This class evaluates the basic expressions given in the interface.
 */


class DictEngine: public Plasma::DataEngine
{
    Q_OBJECT
public:
    DictEngine( QObject* parent, const QVariantList& args );

protected:
    bool sourceRequestEvent(const QString &word);

private:
    void setError(const QString &query, const QString &message);

private Q_SLOTS:
    void slotFinished(KJob *kjob);
};

K_EXPORT_PLASMA_DATAENGINE(dict, DictEngine)

#endif
