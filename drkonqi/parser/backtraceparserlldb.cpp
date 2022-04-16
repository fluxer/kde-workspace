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

#include "backtraceparserlldb.h"
#include "backtraceparser_p.h"

#include <QDebug>

#include <stdio.h>

//BEGIN BacktraceLineLldb

class BacktraceLineLldb : public BacktraceLine
{
public:
    BacktraceLineLldb(const QString &line);
};

BacktraceLineLldb::BacktraceLineLldb(const QString &line)
    : BacktraceLine()
{
    d->m_line = line;
    d->m_type = BacktraceLine::Unknown;
    d->m_rating = BacktraceLine::MissingEverything;

    if (line.trimmed().isEmpty()) {
        d->m_type = BacktraceLine::EmptyLine;
        return;
    }

    if (line.startsWith(QLatin1String("* thread"))) {
        // also SignalHandlerStart
        d->m_type = BacktraceLine::ThreadIndicator;
        d->m_rating = BacktraceLine::Good;
    } else if (line.contains(QLatin1String(" thread #"))) {
        d->m_type = BacktraceLine::ThreadStart;
        d->m_rating = BacktraceLine::Good;
    } else if (line.contains(QLatin1String(" frame #"))) {
        d->m_type = BacktraceLine::StackFrame;
        d->m_rating = BacktraceLine::Good;
    } else {
        d->m_type = BacktraceLine::Crap;
    }
    // qDebug() << Q_FUNC_INFO << line << d->m_type << d->m_rating;
}

//END BacktraceLineLldb

//BEGIN BacktraceParserLldb

class BacktraceParserLldbPrivate : public BacktraceParserPrivate
{
public:
    BacktraceParserLldbPrivate();
};


BacktraceParserLldbPrivate::BacktraceParserLldbPrivate()
    : BacktraceParserPrivate()
{
}


BacktraceParserLldb::BacktraceParserLldb(QObject *parent)
    : BacktraceParser(parent)
{
}

BacktraceParserPrivate* BacktraceParserLldb::constructPrivate() const
{
    return new BacktraceParserLldbPrivate();
}

void BacktraceParserLldb::newLine(const QString &line)
{
    Q_D(BacktraceParserLldb);
    BacktraceLineLldb lldbline(line);
    d->m_linesList.append(lldbline);
    d->m_linesToRate.append(lldbline);
}

//END BacktraceParserLldb

#include "moc_backtraceparserlldb.cpp"
