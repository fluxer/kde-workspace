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

#ifndef BACKTRACEPARSERLLDB_H
#define BACKTRACEPARSERLLDB_H

#include "backtraceparser.h"

class BacktraceParserLldbPrivate;

class BacktraceParserLldb : public BacktraceParser
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BacktraceParserLldb)
public:
    explicit BacktraceParserLldb(QObject *parent = 0);

protected:
    virtual BacktraceParserPrivate *constructPrivate() const;

protected Q_SLOTS:
    virtual void newLine(const QString &line);
};

#endif // BACKTRACEPARSERLLDB_H
