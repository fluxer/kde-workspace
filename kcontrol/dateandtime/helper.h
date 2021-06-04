/*
 *  main.h
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef CLOCK_HELPER_H
#define CLOCK_HELPER_H

#include <kauthactionreply.h>

using namespace KAuth;

class ClockHelper : public QObject
{
    Q_OBJECT

    public:
        enum CH_Error {
            NoError         = 0,
            CallError       = 1,
            TimezoneError   = 2,
            NTPError        = 4,
            DateError       = 8
        };

    public slots:
        ActionReply save(const QVariantMap &map);

    private:
        CH_Error ntp(const QStringList& ntpServers, bool ntpEnabled);
        CH_Error date(const QString& newdate, const QString& olddate);
        CH_Error tz(const QString& selectedzone);
        CH_Error tzreset();
};

#endif // CLOCK_HELPER_H
