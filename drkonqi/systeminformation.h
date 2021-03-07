/*******************************************************************
* systeminformation.h
* Copyright 2009    Dario Andres Rodriguez <andresbajotierra@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************/

#ifndef SYSTEMINFORMATION__H
#define SYSTEMINFORMATION__H

#include <QtCore/QObject>

class SystemInformation: public QObject
{
    Q_OBJECT
    public:
        explicit SystemInformation(QObject * parent = 0);
        ~SystemInformation();

        QString system() const;
        QString release() const;

        QString kdeVersion() const;
        QString qtVersion() const;

    private:
        QString fetchOSDetailInformation() const;
        QString fetchOSReleaseInformation() const;

        QString     m_system;
        QString     m_release;
};

#endif
