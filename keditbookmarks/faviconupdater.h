/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Kellett <lypanov@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef FAVICONUPDATER_H
#define FAVICONUPDATER_H

#include <kbookmark.h>
#include "favicon_interface.h" // org::kde::FavIcon

#include <kparts/part.h>

class FavIconUpdater : public QObject
{
    Q_OBJECT

public:
    FavIconUpdater(QObject *parent);
    void downloadIcon(const KBookmark &bk);

private Q_SLOTS:
    void notifyChange(const QString &url, const QString& iconName);

Q_SIGNALS:
    void done(bool succeeded);

private:
    KBookmark m_bk;
    org::kde::FavIcon m_favIconModule;
};

#endif

