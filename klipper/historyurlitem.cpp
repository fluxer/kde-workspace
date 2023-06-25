/* This file is part of the KDE project
   Copyright (C) 2004  Esben Mose Hansen <kde@mosehansen.dk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "klipper.h"
#include "historyurlitem.h"

#include <QtCore/QMimeData>
#include <QCryptographicHash>

namespace {
    QByteArray compute_uuid(const KUrl::List& _urls, bool _cut ) {
        QCryptographicHash hash(KlipperHashAlgorithm);
        foreach(const KUrl& url, _urls) {
            hash.addData(url.toEncoded());
            hash.addData("\0", 1); // Use binary zero as that is not a valid path character
        }
        return hash.result();
    }
}

HistoryURLItem::HistoryURLItem( const KUrl::List& _urls, bool _cut )
    : HistoryItem(compute_uuid(_urls, _cut))
    , m_urls( _urls )
    , m_cut( _cut )
{
}

/* virtual */
void HistoryURLItem::write( QDataStream& stream ) const
{
    stream << QString( "url" ) << m_urls << (int)m_cut;
}

QString HistoryURLItem::text() const {
    return m_urls.toStringList().join( " " );
}

QMimeData* HistoryURLItem::mimeData() const {
    QMimeData *data = new QMimeData();
    m_urls.populateMimeData(data);
    data->setData("application/x-kde-cutselection", QByteArray(m_cut ? "1" : "0"));
    return data;
}

bool HistoryURLItem::operator==( const HistoryItem& rhs) const
{
    if ( const HistoryURLItem* casted_rhs = dynamic_cast<const HistoryURLItem*>( &rhs ) ) {
        return casted_rhs->m_urls == m_urls && casted_rhs->m_cut == m_cut;
    }
    return false;
}
