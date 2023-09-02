/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007-2008 Sebastian Kuegler <sebas@kde.org>
 *   Copyright 2008 Dario Freddi <drf54321@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#ifndef POWERMANAGEMENTENGINE_H
#define POWERMANAGEMENTENGINE_H

#include <Plasma/DataEngine>

/**
 * This class provides power management in Plasma applets.
 */
class PowermanagementEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PowermanagementEngine(QObject *parent, const QVariantList &args);

    QStringList sources() const;
    Plasma::Service* serviceForSource(const QString &source);

protected:
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);
    void init();

private:
    QStringList m_sources;
};

#endif // POWERMANAGEMENTENGINE_H
