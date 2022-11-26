/*  This file is part of the KDE libraries
    Copyright (C) 2016 Ivailo Monev <xakepa10@gmail.com>

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

#ifndef KMEDIAPART_H
#define KMEDIAPART_H

#include <QPointer>
#include <KParts/Part>
#include <KMediaWidget>
#include <KUrl>

class KMediaPlayerPart : public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    KMediaPlayerPart(QWidget *parentWidget, QObject *parent, const QList<QVariant> &arguments);
    ~KMediaPlayerPart();

    // reimplementations
    bool openFile() final;
    bool closeUrl() final;
public Q_SLOTS:
    bool openUrl(const KUrl &url) final;

private:
    QPointer<KMediaWidget> m_player;
};

#endif // KMEDIAPART_H
