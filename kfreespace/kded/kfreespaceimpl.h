/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

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

#ifndef KFREESPACEIMPL_H
#define KFREESPACEIMPL_H

#include <QObject>
#include <QTimerEvent>

class KFreeSpaceImpl : public QObject
{
    Q_OBJECT
public:
    KFreeSpaceImpl(QObject *parent = nullptr);
    ~KFreeSpaceImpl();

    bool watch(const QString &dirpath,
               const qulonglong checktime, const qulonglong freespace);

protected:
    // reimplementation
    void timerEvent(QTimerEvent *event);

private:
    QString m_directory;
    qulonglong m_checktime;
    qulonglong m_freespace;
    int m_timerid;
};

#endif // KFREESPACEIMPL_H
