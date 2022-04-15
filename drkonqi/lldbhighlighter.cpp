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

#include "lldbhighlighter.h"

#include <QDebug>
#include <KColorScheme>

LldbHighlighter::LldbHighlighter(QTextDocument* parent, const QList<BacktraceLine> &lines)
    : QSyntaxHighlighter(parent)
{
    m_lines = lines;

    KColorScheme kcolorscheme(QPalette::Active);
    m_crapformat.setForeground(kcolorscheme.foreground(KColorScheme::InactiveText));
    m_idformat.setForeground(kcolorscheme.foreground(KColorScheme::PositiveText));
    m_hexformat.setForeground(kcolorscheme.foreground(KColorScheme::NegativeText));
    m_hexformat.setFontWeight(QFont::Bold);
    m_libraryformat.setForeground(kcolorscheme.foreground(KColorScheme::NeutralText));
    m_functionformat.setForeground(kcolorscheme.foreground(KColorScheme::VisitedText));
    m_functionformat.setFontWeight(QFont::Bold);
    m_sourceformat.setForeground(kcolorscheme.foreground(KColorScheme::LinkText));
}

void LldbHighlighter::highlightBlock(const QString &text)
{
    // qDebug() << Q_FUNC_INFO << text << currentBlock().position() << currentBlock().length();
    if (!text.contains(QLatin1String(" thread #"))
        && !text.contains(QLatin1String(" frame #"))) {
        setFormat(0, text.length(), m_crapformat);
    }

    int partlength = 0;
    int partscounter = 0;
    const QStringList textparts = text.split(QLatin1Char(' '));
    foreach (const QString &textpart, textparts) {
        if (textpart.startsWith(QLatin1Char('#'))) {
            const bool lastcharislon = (textpart.length() > 0 && textpart.at(textpart.length() - 1).isLetterOrNumber());
            setFormat(partlength, textpart.length() - int(!lastcharislon), m_idformat);
        } else if (textpart.startsWith(QLatin1String("0x"))) {
            setFormat(partlength, textpart.length(), m_hexformat);
        } else if (textpart.contains(QLatin1Char('`'))) {
            const QStringList subtextpart = textpart.split(QLatin1Char('`'));
            if (subtextpart.size() >= 2) {
                const int firstpartlength = subtextpart.at(0).length();
                setFormat(partlength, firstpartlength, m_libraryformat);
                // TODO: setFormat(partlength + firstpartlength, subtextpart.at(1).length(), m_functionformat);
            }
        } else if (partscounter > 0 && textparts.at(partscounter - 1) == QLatin1String("at")) {
            setFormat(partlength, textpart.length(), m_sourceformat);
        }
        partlength += (textpart.length() + 1);
        partscounter += 1;
    }
}
