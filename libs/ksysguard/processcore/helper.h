/*
    KSysGuard, the KDE System Guard

	Copyright (c) 2009 John Tapsell <john.tapsell@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include <kauthorization.h>

/* The functions here run as ROOT.  So be careful. */

class KSysGuardProcessListHelper : public KAuthorization
{
    Q_OBJECT
public:
    KSysGuardProcessListHelper(const char* const helper, QObject *parent = nullptr);
public Q_SLOTS:
    int renice(QVariantMap parameters);
    int changecpuscheduler(QVariantMap parameters);
};

Q_DECLARE_METATYPE( QList<long long> )
