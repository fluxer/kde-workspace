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

#ifndef KFREESPACE_H
#define KFREESPACE_H

static const bool s_kfreespacewatch = true;
static const qulonglong s_kfreespacechecktime = 60; // 1 minute
static const qulonglong s_kfreespacechecktimemin = 1;
static const qulonglong s_kfreespacechecktimemax = 60;
static const qulonglong s_kfreespacefreespace = 1024; // 1 GB
static const qulonglong s_kfreespacefreespacemin = 10;
static const qulonglong s_kfreespacefreespacemax = 1024;

#endif // KFREESPACE_H
