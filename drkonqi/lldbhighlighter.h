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

#ifndef LLDBHIGHLIGHTER_H
#define LLDBHIGHLIGHTER_H

#include "parser/backtraceline.h"

#include <QSyntaxHighlighter>
#include <QTextDocument>

class LldbHighlighter : public QSyntaxHighlighter
{
public:
    LldbHighlighter(QTextDocument* parent, const QList<BacktraceLine> &lines);

protected:
    void highlightBlock(const QString& text) final;

private:
    QTextCharFormat m_crapformat;
    QTextCharFormat m_signalformat;
    QTextCharFormat m_idformat;
    QTextCharFormat m_hexformat;
    QTextCharFormat m_libraryformat;
    QTextCharFormat m_functionformat;
    QTextCharFormat m_sourceformat;
};

#endif // LLDBHIGHLIGHTER_H
