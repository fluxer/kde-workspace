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

#include "kdiffhighlighter.h"

#include <kcolorscheme.h>
#include <kcolorutils.h>
#include <kdebug.h>

// NOTE: kate does this
static QColor adaptColor(const qreal foregroundluma, const qreal backgroundluma, const QColor &foregroundcolor, const QColor &color)
{
    if (foregroundluma > backgroundluma) {
        return KColorUtils::tint(foregroundcolor, color, 0.5);
    }
    return color;
}

KDiffHighlighter::KDiffHighlighter(QTextEdit *parent)
    : QSyntaxHighlighter(parent)
{
    const KColorScheme kcolorscheme = KColorScheme(QPalette::Active, KColorScheme::View);
    const QColor foregroundcolor = kcolorscheme.foreground().color();
    const qreal foregroundluma = KColorUtils::luma(foregroundcolor);
    const qreal backgroundluma = KColorUtils::luma(kcolorscheme.background().color());

    const QColor oldcolor = QColor(QString::fromLatin1("#FF0000"));
    m_oldformat.setForeground(adaptColor(foregroundluma, backgroundluma, foregroundcolor, oldcolor));
    const QColor newcolor = QColor(QString::fromLatin1("#0000FF"));
    m_newformat.setForeground(adaptColor(foregroundluma, backgroundluma, foregroundcolor, newcolor));
}

void KDiffHighlighter::highlightBlock(const QString &text)
{
    // qDebug() << Q_FUNC_INFO << text;
    if (text.startsWith(QLatin1Char('-'))) {
        setFormat(0, text.size(), m_oldformat);
    } else if (text.startsWith(QLatin1Char('+'))) {
        setFormat(0, text.size(), m_newformat);
    } else {
        setFormat(0, text.size(), QTextCharFormat());
    }
}

#include "moc_kdiffhighlighter.cpp"
