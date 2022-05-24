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

#ifndef FAVICONTEST_H
#define FAVICONTEST_H

#include "favicon_interface.h"

#include <QObject>

class FavIconTest : public QObject
{
    Q_OBJECT
public:
    FavIconTest();

private Q_SLOTS:
    void initTestCase();
    void testSetIconForURL_data();
    void testSetIconForURL();
    void testIconForURL_data();
    void testIconForURL();

    void slotIconChanged(const bool isHost, const QString &hostOrURL, const QString &iconName);
    void slotInfoMessage(const QString &iconURL, const QString &msg);
    void slotError(const bool isHost, const QString &hostOrURL, const QString &errorString);

private:
    bool m_iconChanged;
    bool m_isHost;
    QString m_hostOrURL;
    QString m_iconName;

    org::kde::FavIcon m_favIconModule;
};


#endif /* FAVICONTEST_H */

