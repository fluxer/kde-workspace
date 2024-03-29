/*   Kate search plugin
 * 
 * Copyright (C) 2011-2013 by Kåre Särs <kare.sars@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file called COPYING; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "search_open_files.h"
#include "moc_search_open_files.cpp"

#include <QtCore/qdatetime.h>

SearchOpenFiles::SearchOpenFiles(QObject *parent) : QObject(parent), m_nextIndex(-1), m_cancelSearch(true)
{
    connect(this, SIGNAL(searchNextFile(int)), this, SLOT(doSearchNextFile(int)), Qt::QueuedConnection);
}

bool SearchOpenFiles::searching() { return !m_cancelSearch; }

void SearchOpenFiles::startSearch(const QList<KTextEditor::Document*> &list, const QRegExp &regexp)
{
    if (m_nextIndex != -1) return;

    m_docList = list;
    m_nextIndex = 0;
    m_regExp = regexp;
    m_cancelSearch = false;
    m_statusTime.restart();
    emit searchNextFile(0);
}

void SearchOpenFiles::cancelSearch()
{
    m_cancelSearch = true;
}

void SearchOpenFiles::doSearchNextFile(int startLine)
{
    if (m_cancelSearch) {
        m_nextIndex = -1;
        m_cancelSearch = true;
        emit searchDone();
        return;
    }

    // NOTE The document managers signal documentWillBeDeleted() must be connected to
    // cancelSearch(). A closed file could lead to a crash if it is not handled.
    int line = searchOpenFile(m_docList[m_nextIndex], m_regExp, startLine);
    if (line == 0) {
        // file searched go to next
        m_nextIndex++;
        if (m_nextIndex == m_docList.size()) {
            m_nextIndex = -1;
            m_cancelSearch = true;
            emit searchDone();
        }
        else {
            emit searchNextFile(0);
        }
    }
    else {
        emit searchNextFile(line);
    }
}

int SearchOpenFiles::searchOpenFile(KTextEditor::Document *doc, const QRegExp &regExp, int startLine)
{
    if (m_statusTime.elapsed() > 100) {
        m_statusTime.restart();
        emit searching(doc->url().pathOrUrl());
    }

    if (regExp.pattern().contains("\\n")) {
        return searchMultiLineRegExp(doc, regExp, startLine);
    }

    return searchSingleLineRegExp(doc, regExp, startLine);
}

int SearchOpenFiles::searchSingleLineRegExp(KTextEditor::Document *doc, const QRegExp &regExp, int startLine)
{
    int column;
    QElapsedTimer time;

    time.start();
    for (int line = startLine; line < doc->lines(); line++) {
        if (time.elapsed() > 100) {
            kDebug() << "Search time exceeded" << time.elapsed() << line;
            return line;
        }
        column = regExp.indexIn(doc->line(line));
        while (column != -1) {
            if (regExp.cap().isEmpty()) break;
            emit matchFound(doc->url().pathOrUrl(), doc->documentName(), line, column,
                            doc->line(line), regExp.matchedLength());
            column = regExp.indexIn(doc->line(line), column + regExp.cap().size());
        }
    }
    return 0;
}

int SearchOpenFiles::searchMultiLineRegExp(KTextEditor::Document *doc, const QRegExp &regExp, int startLine)
{
    int column = 0;
    int line = 0;
    QElapsedTimer time;
    time.start();
    QRegExp tmpRegExp = regExp;

    if (startLine == 0) {
        // Copy the whole file to a temporary buffer to be able to search newlines
        m_fullDoc.clear();
        m_lineStart.clear();
        m_lineStart << 0;
        for (int i=0; i<doc->lines(); i++) {
            m_fullDoc += doc->line(i) + '\n';
            m_lineStart << m_fullDoc.size();
        }
        if (!regExp.pattern().endsWith("$")) {
            // if regExp ends with '$' leave the extra newline at the end as
            // '$' will be replaced with (?=\\n), which needs the extra newline
            m_fullDoc.remove(m_fullDoc.size()-1, 1);
        }
    }
    else {
        if (startLine>0 && startLine<m_lineStart.size()) {
            column = m_lineStart[startLine];
            line = startLine;
        }
        else {
            return 0;
        }
    }

    if (regExp.pattern().endsWith("$")) {
        QString newPatern = tmpRegExp.pattern();
        newPatern.replace("$", "(?=\\n)");
        tmpRegExp.setPattern(newPatern);
    }

    column = tmpRegExp.indexIn(m_fullDoc, column);
    while (column != -1) {
        if (tmpRegExp.cap().isEmpty()) break;
        // search for the line number of the match
        int i;
        line = -1;
        for (i=1; i<m_lineStart.size(); i++) {
            if (m_lineStart[i] > column) {
                line = i-1;
                break;
            }
        }
        if (line == -1) {
            break;
        }
        emit matchFound(doc->url().pathOrUrl(), doc->documentName(),
                        line,
                        (column - m_lineStart[line]),
                        doc->line(line).left(column - m_lineStart[line])+tmpRegExp.cap(),
                        tmpRegExp.matchedLength());
        column = tmpRegExp.indexIn(m_fullDoc, column + tmpRegExp.matchedLength());

        if (time.elapsed() > 100) {
            //kDebug() << "Search time exceeded" << time.elapsed() << line;
            return line;
        }
    }
    return 0;
}
