/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
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

#ifndef TIMEENGINE_H
#define TIMEENGINE_H

#include <Plasma/DataEngine>

/**
 * This engine provides the current date and time for a given
 * timezone. Optionally it can also provide solar position info.
 *
 * "Local" is a special source that is an alias for the current
 * timezone.
 */
class TimeEngine : public Plasma::DataEngine
{
    Q_OBJECT

    public:
        TimeEngine(QObject *parent, const QVariantList &args);
        ~TimeEngine();

        void init();
        QStringList sources() const;

    protected:
        bool sourceRequestEvent(const QString &name);
        bool updateSourceEvent(const QString &source);

    protected Q_SLOTS:
        void clockSkewed(); // call when system time changed and all clocks should be updated
        void checkTZ();

    private:
        QString m_tz;
};

#endif // TIMEENGINE_H
