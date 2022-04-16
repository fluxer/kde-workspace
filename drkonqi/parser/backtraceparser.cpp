/*
    Copyright (C) 2009-2010 George Kiagiadakis <kiagiadakis.george@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "backtraceparser_p.h"
#include "backtraceparsergdb.h"
#include "backtraceparserlldb.h"
#include "backtraceparsernull.h"
#include <QtCore/QRegExp>
#include <QtCore/qmetaobject.h>
#include <KDebug>

//factory
BacktraceParser *BacktraceParser::newParser(const QString & debuggerName, QObject *parent)
{
    if (debuggerName == "gdb") {
        return new BacktraceParserGdb(parent);
    } else if (debuggerName == "lldb") {
        return new BacktraceParserLldb(parent);
    } else {
        return new BacktraceParserNull(parent);
    }
}

BacktraceParser::BacktraceParser(QObject *parent) : QObject(parent), d_ptr(NULL) {}
BacktraceParser::~BacktraceParser() { delete d_ptr; }

void BacktraceParser::connectToGenerator(QObject *generator)
{
    connect(generator, SIGNAL(starting()), this, SLOT(resetState()));
    connect(generator, SIGNAL(newLine(QString)), this, SLOT(newLine(QString)));
}

QString BacktraceParser::parsedBacktrace() const
{
    Q_D(const BacktraceParser);

    QString result;
    if (d) {
        foreach(const BacktraceLine i, d->m_linesList) {
            result += i.toString();
        }
    }
    return result;
}

QList<BacktraceLine> BacktraceParser::parsedBacktraceLines() const
{
    Q_D(const BacktraceParser);
    return d ? d->m_linesList : QList<BacktraceLine>();
}

BacktraceParser::Usefulness BacktraceParser::backtraceUsefulness() const
{
    Q_D(const BacktraceParser);

    //if there is no cached usefulness, the data calculation function has not run yet.
    if (d && d->m_usefulness == InvalidUsefulness) {
        const_cast<BacktraceParser*>(this)->calculateRatingData();
    }

    //if there is no d, the debugger has not run yet,
    //so we can say that the (inexistent) backtrace is Useless.
    return d ? d->m_usefulness : Useless;
}

QSet<QString> BacktraceParser::librariesWithMissingDebugSymbols() const
{
    Q_D(const BacktraceParser);

    //if there is no cached usefulness, the data calculation function has not run yet.
    if (d && d->m_usefulness == InvalidUsefulness) {
        const_cast<BacktraceParser*>(this)->calculateRatingData();
    }

    //if there is no d, the debugger has not run yet, so we have no libraries.
    return d ? d->m_librariesWithMissingDebugSymbols : QSet<QString>();
}

void BacktraceParser::resetState()
{
    //reset the state of the parser by getting a new instance of Private
    delete d_ptr;
    d_ptr = constructPrivate();
}

BacktraceParserPrivate *BacktraceParser::constructPrivate() const
{
    return new BacktraceParserPrivate();
}


/* This function returns true if the given stack frame line is the base of the backtrace
   and thus the parser should not rate any frames below that one. */
static bool lineIsStackBase(const BacktraceLine & line)
{
    //optimization. if there is no function name, do not bother to check it
    if ( line.rating() == BacktraceLine::MissingEverything
        || line.rating() == BacktraceLine::MissingFunction )
        return false;

    //this is the base frame for all threads except the main thread
    //FIXME that probably works only on linux
    if ( line.functionName() == "start_thread" )
        return true;

    QRegExp regExp;
    regExp.setPattern("(kde)?main"); //main() or kdemain() is the base for the main thread
    if ( regExp.exactMatch(line.functionName()) )
        return true;

    //HACK for better rating. we ignore all stack frames below any function that matches
    //the following regular expression. The functions that match this expression are usually
    //"QApplicationPrivate::notify_helper", "QApplication::notify" and similar, which
    //are used to send any kind of event to the Qt application. All stack frames below this,
    //with or without debug symbols, are useless to KDE developers, so we ignore them.
    regExp.setPattern("(Q|K)(Core)?Application(Private)?::notify.*");
    if ( regExp.exactMatch(line.functionName()) )
        return true;

    //attempt to recognize crashes that happen after main has returned (bug 200993)
    if ( line.functionName() == "~KCleanUpGlobalStatic" ||
         line.functionName() == "~QGlobalStatic" ||
         line.functionName() == "exit" ||
         line.functionName() == "*__GI_exit" )
        return true;

    return false;
}

/* This function returns true if the given stack frame line is the top of the bactrace
   and thus the parser should not rate any frames above that one. This is used to avoid
   rating the stack frames of abort(), assert(), Q_ASSERT() and qFatal() */
static bool lineIsStackTop(const BacktraceLine & line)
{
    //optimization. if there is no function name, do not bother to check it
    if ( line.rating() == BacktraceLine::MissingEverything
        || line.rating() == BacktraceLine::MissingFunction )
        return false;

    if ( line.functionName().startsWith(QLatin1String("qt_assert")) //qt_assert and qt_assert_x
        || line.functionName() == "qFatal"
        || line.functionName() == "abort"
        || line.functionName() == "*__GI_abort"
        || line.functionName() == "*__GI___assert_fail" )
        return true;

    return false;
}

/* This function returns true if the given stack frame line should be ignored from rating
   for some reason. Currently it ignores all libc/libstdc++/libpthread functions. */
static bool lineShouldBeIgnored(const BacktraceLine & line)
{
    if ( line.libraryName().contains("libc.so")
        || line.libraryName().contains("libstdc++.so")
        || line.functionName().startsWith(QLatin1String("*__GI_")) //glibc2.9 uses *__GI_ as prefix
        || line.libraryName().contains("libpthread.so")
        || line.libraryName().contains("libglib-2.0.so")
        || line.functionName().contains("_tmain") )
        return true;

    return false;
}

void BacktraceParser::calculateRatingData()
{
    Q_D(BacktraceParser);

    uint rating = 0, bestPossibleRating = 0, counter = 0;
    bool haveSeenStackBase = false;

    QListIterator<BacktraceLine> i(d->m_linesToRate);
    i.toBack(); //start from the end of the list

    while( i.hasPrevious() ) {
        const BacktraceLine & line = i.previous();

        if ( !i.hasPrevious() && line.rating() == BacktraceLine::MissingEverything ) {
            //Under some circumstances, the very first stack frame is invalid (ex, calling a function
            //at an invalid address could result in a stack frame like "0x00000000 in ?? ()"),
            //which however does not necessarily mean that the backtrace has a missing symbol on
            //the first line. Here we make sure to ignore this line from rating. (bug 190882)
            break; //there are no more items anyway, just break the loop
        }

        if ( lineIsStackBase(line) ) {
            rating = bestPossibleRating = counter = 0; //restart rating ignoring any previous frames
            haveSeenStackBase = true;
        } else if ( lineIsStackTop(line) ) {
            break; //we have reached the top, no need to inspect any more frames
        }

        if ( lineShouldBeIgnored(line) ) {
            continue;
        }

        if ( line.rating() == BacktraceLine::MissingFunction
            || line.rating() == BacktraceLine::MissingSourceFile) {
            d->m_librariesWithMissingDebugSymbols.insert(line.libraryName().trimmed());
        }

        uint multiplier = ++counter; //give weight to the first lines
        rating += static_cast<uint>(line.rating()) * multiplier;
        bestPossibleRating += static_cast<uint>(BacktraceLine::BestRating) * multiplier;

        kDebug() << line.rating() << line.toString();
    }

    //calculate rating
    d->m_usefulness = Useless;
    if (rating >= (bestPossibleRating*0.90)) {
        d->m_usefulness = ReallyUseful;
    } else if (rating >= (bestPossibleRating*0.70)) {
        d->m_usefulness = MayBeUseful;
    } else if (rating >= (bestPossibleRating*0.40)) {
        d->m_usefulness = ProbablyUseless;
    }

    //if there is no stack base, the executable is probably stripped,
    //so we need to be more strict with rating
    if ( !haveSeenStackBase ) {
        //less than 4 stack frames is useless
        if ( counter < 4 ) {
            d->m_usefulness = Useless;
        //more than 4 stack frames might have some value, so let's not be so strict, just lower the rating
        } else if ( d->m_usefulness > Useless ) {
            d->m_usefulness = (Usefulness) (d->m_usefulness - 1);
        }
    }

    kDebug() << "Rating:" << rating << "out of" << bestPossibleRating << "Usefulness:"
             << staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("Usefulness")).valueToKey(d->m_usefulness);
    kDebug() << "90%:" << (bestPossibleRating*0.90) << "70%:" << (bestPossibleRating*0.70)
             << "40%:" << (bestPossibleRating*0.40);
    kDebug() << "Have seen stack base:" << haveSeenStackBase << "Lines counted:" << counter;
}

#include "moc_backtraceparser.cpp"
