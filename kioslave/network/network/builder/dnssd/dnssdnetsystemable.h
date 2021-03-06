/*
    This file is part of the Mollet network library, part of the KDE project.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DNSSDNETSYSTEMABLE_H
#define DNSSDNETSYSTEMABLE_H

// KDE
#include <DNSSD/RemoteService>
// Qt
#include <QtCore/qplugin.h>

namespace Mollet {
class NetServicePrivate;
class NetDevice;
}
#include <QString>


namespace Mollet
{

class DNSSDNetSystemAble
{
  public:
    virtual ~DNSSDNetSystemAble();

  public: // API to be implemented
    virtual bool canCreateNetSystemFromDNSSD( const QString& serviceType ) const = 0;
    virtual NetServicePrivate* createNetService( const DNSSD::RemoteService::Ptr& service, const NetDevice& device ) const = 0;
    virtual QString dnssdId( const DNSSD::RemoteService::Ptr& dnssdService ) const = 0;
};


inline DNSSDNetSystemAble::~DNSSDNetSystemAble() {}

}

Q_DECLARE_INTERFACE( Mollet::DNSSDNetSystemAble, "org.kde.mollet.dnssdnetsystemable/1.0" )

#endif
