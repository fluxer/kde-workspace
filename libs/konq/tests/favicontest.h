/* This file is part of KDE
    Copyright (c) 2006 David Faure <faure@kde.org>

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

#ifndef FAVICONTEST_H
#define FAVICONTEST_H

#include <QObject>
#include "favicon_interface.h"

class FavIconTest : public QObject
{
    Q_OBJECT
public:
    FavIconTest();

private Q_SLOTS:
    void initTestCase();
    void testSetIconForURL();
    void testIconForURL();
    //void testDownloadHostIcon();

private:
    void waitForSignal();
    org::kde::FavIcon m_favIconModule;
};


#endif /* FAVICONTEST_H */

