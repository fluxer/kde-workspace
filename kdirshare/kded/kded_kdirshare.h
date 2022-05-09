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

#ifndef KDIRSHARE_KDED_H
#define KDIRSHARE_KDED_H

#include <QList>
#include <kdedmodule.h>
#include "kdirshareimpl.h"

class KDirShareModule: public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdirshare")

public:
    KDirShareModule(QObject *parent, const QList<QVariant>&);
    ~KDirShareModule();

public Q_SLOTS:
    Q_SCRIPTABLE QString share(const QString &dirpath);
    Q_SCRIPTABLE QString unshare(const QString &dirpath);

    Q_SCRIPTABLE bool isShared(const QString &dirpath) const;

private:
    QList<KDirShareImpl*> m_dirshares;
};

#endif // KDIRSHARE_KDED_H
