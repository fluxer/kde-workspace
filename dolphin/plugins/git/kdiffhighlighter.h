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

#ifndef KDIFFHIGHLIGHTER_H
#define KDIFFHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QTextCharFormat>

class KDiffHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    KDiffHighlighter(QTextEdit *parent);

protected:
    // QSyntaxHighlighter reimplementation
    void highlightBlock(const QString &text) final;

private:
    QTextCharFormat m_oldformat;
    QTextCharFormat m_newformat;
};

#endif // KDIFFHIGHLIGHTER_H

