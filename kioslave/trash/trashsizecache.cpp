/*
   This file is part of the KDE project

   Copyright (C) 2009 Tobias Koenig <tokoe@kde.org>

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

#include "trashsizecache.h"

#include "discspaceutil.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klockfile.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <QtCore/QDir>

static QString kTrashLock()
{
    return KGlobal::dirs()->saveLocation("tmp") + QLatin1String( "trash" );
}

TrashSizeCache::TrashSizeCache( const QString &path )
    : mTrashSizeCachePath( path + QDir::separator() + QString::fromLatin1( "metadata" ) ),
      mTrashPath( path ),
      mTrashSizeGroup( QLatin1String( "Cached" ) ),
      mTrashSizeKey( QLatin1String( "Size" ) )
{
}

void TrashSizeCache::initialize()
{
    // we call just currentSize here, as it does the initialization for us
    currentSize( true );
}

void TrashSizeCache::add( qulonglong value )
{
    KLockFile lock( kTrashLock() );
    lock.lock();

    KConfig config( mTrashSizeCachePath );
    KConfigGroup group = config.group( mTrashSizeGroup );

    qulonglong size = currentSize( false );
    size += value;

    group.writeEntry( mTrashSizeKey, size );
    config.sync();
}

void TrashSizeCache::remove( qulonglong value )
{
    KLockFile lock( kTrashLock() );
    lock.lock();

    KConfig config( mTrashSizeCachePath );
    KConfigGroup group = config.group( mTrashSizeGroup );

    qulonglong size = currentSize( false );
    size -= value;

    group.writeEntry( mTrashSizeKey, size );
    config.sync();
}

void TrashSizeCache::clear()
{
    KLockFile lock( kTrashLock() );
    lock.lock();

    KConfig config( mTrashSizeCachePath );
    KConfigGroup group = config.group( mTrashSizeGroup );

    group.writeEntry( mTrashSizeKey, (qulonglong)0 );
    config.sync();
}

qulonglong TrashSizeCache::size() const
{
    return currentSize( true );
}

qulonglong TrashSizeCache::currentSize( bool doLocking ) const
{
    KLockFile lock( kTrashLock() );

    if ( doLocking ) {
        lock.lock();
    }

    KConfig config( mTrashSizeCachePath );
    KConfigGroup group = config.group( mTrashSizeGroup );

    if ( !group.hasKey( mTrashSizeKey ) ) {
        // For the first call to the trash size cache, we have to calculate
        // the current size.
        const qulonglong size = DiscSpaceUtil::sizeOfPath( mTrashPath + QString::fromLatin1( "/files/" ) );

        group.writeEntry( mTrashSizeKey, size );
        config.sync();
    }

    const qulonglong value = group.readEntry( mTrashSizeKey, (qulonglong)0 );

    if ( doLocking ) {
        lock.unlock();
    }

    return value;
}
