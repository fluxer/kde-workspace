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

    const QString trimmedline = line.trimmed();
    if (trimmedline.isEmpty()) {
        d->m_type = BacktraceLine::EmptyLine;
        return;
    }

    if (line.startsWith(QLatin1String("* thread"))) {
        // also SignalHandlerStart
        d->m_type = BacktraceLine::ThreadIndicator;
        d->m_rating = BacktraceLine::MissingEverything;
    } else if (line.contains(QLatin1String(" thread #"))) {
        d->m_type = BacktraceLine::ThreadStart;
        d->m_rating = BacktraceLine::MissingEverything;
    } else if (line.contains(QLatin1String(" frame #"))) {
        d->m_type = BacktraceLine::StackFrame;
        d->m_rating = BacktraceLine::Good;

        int partscounter = 0;
        const QStringList lineparts = trimmedline.split(QLatin1Char(' '));
        foreach (const QString &linepart, lineparts) {
            if (linepart.contains(QLatin1Char('`'))) {
                const int tildeindex = linepart.indexOf(QLatin1Char('`'));
                d->m_library = linepart.mid(0, tildeindex);
                d->m_functionName = linepart.mid(tildeindex + 1, linepart.length() - tildeindex + 1);
                const int bracketindex = d->m_functionName.indexOf(QLatin1Char('('));
                if (bracketindex > 0) {
                    d->m_functionName = d->m_functionName.mid(0, bracketindex);
                }
            } else if (partscounter > 0 && lineparts.at(partscounter - 1) == QLatin1String("at")) {
                d->m_file = linepart;
            }
            partscounter += 1;
        }

        if (d->m_functionName.isEmpty()) {
            d->m_rating = BacktraceLine::MissingFunction;
        }
        if (d->m_library.isEmpty()) {
            d->m_rating = BacktraceLine::MissingLibrary;
        }
        if (d->m_file.isEmpty()) {
            d->m_rating = BacktraceLine::MissingSourceFile;
        }
        // qDebug() << Q_FUNC_INFO << line << d->m_functionName << d->m_library << d->m_file;
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
    switch (lldbline.type()) {
        case BacktraceLine::Unknown:
        case BacktraceLine::EmptyLine:
        case BacktraceLine::Crap:
        case BacktraceLine::KCrash: {
            break;
        }
        case BacktraceLine::ThreadIndicator:
        case BacktraceLine::ThreadStart:
        case BacktraceLine::SignalHandlerStart:
        case BacktraceLine::StackFrame: {
            d->m_linesToRate.append(lldbline);
            break;
        }
    }
}

//END BacktraceParserLldb

#include "moc_backtraceparserlldb.cpp"
