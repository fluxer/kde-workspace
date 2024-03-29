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

#include "replace_matches.h"
#include "moc_replace_matches.cpp"
#include <QtGui/qtreewidget.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrange.h>
#include <klocale.h>

ReplaceMatches::ReplaceMatches(QObject *parent) : QObject(parent),
m_manager(0),
m_tree(0),
m_rootIndex(-1)
{
    connect(this, SIGNAL(replaceNextMatch()), this, SLOT(doReplaceNextMatch()), Qt::QueuedConnection);
}

void ReplaceMatches::replaceChecked(QTreeWidget *tree, const QRegExp &regexp, const QString &replace)
{
    if (m_manager == 0) return;
    if (m_rootIndex != -1) return;
    
    m_tree = tree;
    m_rootIndex = 0;
    m_regExp = regexp;
    m_replaceText = replace;
    m_cancelReplace = false;
    emit replaceNextMatch();
}
void ReplaceMatches::setDocumentManager(Kate::DocumentManager *manager)
{
    m_manager = manager;
}

void ReplaceMatches::cancelReplace()
{
    m_cancelReplace = true;
}

KTextEditor::Document *ReplaceMatches::findNamed(const QString &name)
{
    QList<KTextEditor::Document*> docs = m_manager->documents();

    foreach (KTextEditor::Document* it, docs) {
        if ( it->documentName() == name) {
            return it;
        }
    }
    return 0;
}


void ReplaceMatches::doReplaceNextMatch()
{
    if ((!m_manager) || (m_cancelReplace) || (m_tree->topLevelItemCount() != 1)) {
        m_rootIndex = -1;
        emit replaceDone();
        return;
    }

    // NOTE The document managers signal documentWillBeDeleted() must be connected to
    // cancelReplace(). A closed file could lead to a crash if it is not handled.

    // Open the file
    QTreeWidgetItem *rootItem = m_tree->topLevelItem(0)->child(m_rootIndex);
    if (!rootItem) {
        m_rootIndex = -1;
        emit replaceDone();
        return;
    }

    if (!rootItem->data(0, ColumnRole).toString().isEmpty()) {
        // this is a search as you type replace
        rootItem = m_tree->topLevelItem(0);
        m_cancelReplace = true; // only one document...
    }

    if (rootItem->checkState(0) == Qt::Unchecked) {
        m_rootIndex++;
        emit replaceNextMatch();
        return;
    }

    KTextEditor::Document *doc;
    QString docUrl = rootItem->data(0, FileUrlRole).toString();
    QString docName = rootItem->data(0, FileNameRole).toString();
    if (docUrl.isEmpty()) {
        doc = findNamed(rootItem->data(0, FileNameRole).toString());
    }
    else {
        doc = m_manager->findUrl(docUrl);
        if (!doc) {
            doc = m_manager->openUrl(rootItem->data(0, FileUrlRole).toString());
        }
    }

    if (!doc) {
        m_rootIndex++;
        emit replaceNextMatch();
        return;
    }

    QVector<KTextEditor::MovingRange*> rVector;
    QStringList rTexts;
    KTextEditor::MovingInterface* miface = qobject_cast<KTextEditor::MovingInterface*>(doc);
    int line;
    int column;
    int matchLen;
    int endLine;
    int endColumn;
    QTreeWidgetItem *item;
    QString matchLines;

    // lines might be modified so search the document again
    for (int i=0; i<rootItem->childCount(); i++) {
        item = rootItem->child(i);
        if (item->checkState(0) == Qt::Unchecked) continue;

        line = endLine= item->data(0, LineRole).toInt();
        column = item->data(0, ColumnRole).toInt();
        matchLen = item->data(0, MatchLenRole).toInt();
        matchLines = doc->line(line).mid(column);
        while (matchLines.size() < matchLen) {
            if (endLine+1 >= doc->lines()) break;
            endLine++;
            matchLines+= '\n' + doc->line(endLine);
        }

        if (m_regExp.indexIn(matchLines) != 0) {
            kDebug() << "expression does not match";
            continue;
        }

        QString replaceText = m_replaceText;
        replaceText.replace("\\\\", "¤Search&Replace¤");
        for (int j=1; j<=m_regExp.captureCount(); j++) {
            replaceText.replace(QString("\\%1").arg(j), m_regExp.cap(j));
        }
        replaceText.replace("\\n", "\n");
        replaceText.replace("¤Search&Replace¤", "\\\\");
        rTexts << replaceText;

        replaceText.replace('\n', "\\n");
        QString html = item->data(0, PreMatchRole).toString();
        html += "<i><s>" + item->data(0, MatchRole).toString() + "</s></i> ";
        html += "<b>" + replaceText + "</b>";
        html += item->data(0, PostMatchRole).toString();
        item->setData(0, Qt::DisplayRole, i18n("Line: <b>%1</b>: %2",line+1, html));

        endLine = line;
        endColumn = column+matchLen;
        while ((endLine < doc->lines()) &&  (endColumn > doc->line(endLine).size())) {
            endColumn -= doc->line(endLine).size();
            endColumn--; // remove one for '\n'
            endLine++;
        }
        KTextEditor::Range range(line, column, endLine, endColumn);
        KTextEditor::MovingRange* mr = miface->newMovingRange(range);
        rVector.append(mr);
    }

    for (int i=0; i<rVector.size(); i++) {
        line = rVector[i]->start().line();
        column = rVector[i]->start().column();
        doc->replaceText(rVector[i]->toRange(), rTexts[i]);
        emit matchReplaced(doc, line, column, rTexts[i].length());
    }

    qDeleteAll(rVector);

    m_rootIndex++;
    emit replaceNextMatch();
}
