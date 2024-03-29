// -*- indent-tabs-mode:nil -*-
// vim: set ts=4 sts=4 sw=4 et:
/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2002-2003 Alexander Kellett <lypanov@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 or at your option version 3 as published by
   the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "importers.h"
#include "globalbookmarkmanager.h"

#include "kbookmarkmodel/commands.h"
#include "toplevel.h" // for KEBApp
#include "kbookmarkmodel/model.h"

#include <QtCore/QRegExp>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kbookmark.h>
#include <kbookmarkmanager.h>


ImportCommand::ImportCommand(KBookmarkModel* model)
    : QUndoCommand(), m_model(model), m_utf8(false), m_folder(false), m_cleanUpCmd(0)
{
}

void ImportCommand::setVisibleName(const QString& visibleName)
{
    m_visibleName = visibleName;
    setText(i18nc("(qtundo-format)", "Import %1 Bookmarks", visibleName));
}

QString ImportCommand::folder() const {
    return m_folder ? i18n("%1 Bookmarks", visibleName()) : QString();
}

ImportCommand* ImportCommand::importerFactory(KBookmarkModel* model, const QString &type)
{
    if (type == "KDE2") return new KDE2ImportCommand(model);
    else {
        kError() << "ImportCommand::importerFactory() - invalid type (" << type << ")!";
        return 0;
    }
}

ImportCommand* ImportCommand::performImport(KBookmarkModel* model, const QString &type, QWidget *top)
{
    ImportCommand *importer = ImportCommand::importerFactory(model, type);

    Q_ASSERT(importer);

    QString mydirname = importer->requestFilename();
    if (mydirname.isEmpty()) {
        delete importer;
        return 0;
    }

    int answer =
        KMessageBox::questionYesNoCancel(
                top, i18n("Import as a new subfolder or replace all the current bookmarks?"),
                i18nc("@title:window", "%1 Import", importer->visibleName()),
                KGuiItem(i18n("As New Folder")), KGuiItem(i18n("Replace")));

    if (answer == KMessageBox::Cancel) {
        delete importer;
        return 0;
    }

    importer->import(mydirname, answer == KMessageBox::Yes);
    return importer;
}

void ImportCommand::doCreateHoldingFolder(KBookmarkGroup &bkGroup) {
    bkGroup = GlobalBookmarkManager::self()->mgr()
        ->root().createNewFolder(folder());
    bkGroup.setIcon(m_icon);
    m_group = bkGroup.address();
}

void ImportCommand::redo()
{
    KBookmarkGroup bkGroup;

    if (!folder().isNull()) {
        doCreateHoldingFolder(bkGroup);

    } else {
        // import into the root, after cleaning it up
        bkGroup = GlobalBookmarkManager::self()->root();
        delete m_cleanUpCmd;
        m_cleanUpCmd = DeleteCommand::deleteAll(m_model, bkGroup);

        new DeleteCommand(m_model, bkGroup.address(),
                          true /* contentOnly */, m_cleanUpCmd);
        m_cleanUpCmd->redo();

        // import at the root
        m_group = "";
    }

    doExecute(bkGroup);

    // notify the model that the data has changed
    //
    // FIXME Resetting the model completely has the unwanted
    // side-effect of collapsing all items in tree views
    // (and possibly other side effects)
    m_model->resetModel();
}

void ImportCommand::undo()
{
    if ( !folder().isEmpty() ) {
        // we created a group -> just delete it
        DeleteCommand cmd(m_model, m_group);
        cmd.redo();

    } else {
        // we imported at the root -> delete everything
        KBookmarkGroup root = GlobalBookmarkManager::self()->root();
        QUndoCommand *cmd = DeleteCommand::deleteAll(m_model, root);

        cmd->redo();
        delete cmd;

        // and recreate what was there before
        m_cleanUpCmd->undo();
    }
}

QString ImportCommand::affectedBookmarks() const
{
    QString rootAdr = GlobalBookmarkManager::self()->root().address();
    if(m_group == rootAdr)
        return m_group;
    else
        return KBookmark::parentAddress(m_group);
}

/* -------------------------------------- */

// following is really just xbel
QString KDE2ImportCommand::requestFilename() const {
    return KFileDialog::getOpenFileName(
            KStandardDirs::locateLocal("data", "konqueror"),
            i18n("*.xml|KDE Bookmark Files (*.xml)"),
            KEBApp::self());
}

/* -------------------------------------- */

void XBELImportCommand::doCreateHoldingFolder(KBookmarkGroup &) {
    // rather than reuse the old group node we transform the
    // root xbel node into the group when doing an xbel import
}

void XBELImportCommand::doExecute(const KBookmarkGroup &/*bkGroup*/) {
    // check if already open first???
    KBookmarkManager *pManager = KBookmarkManager::managerForFile(m_fileName, QString());

    QDomDocument doc = GlobalBookmarkManager::self()->mgr()->internalDocument();

    // get the xbel
    QDomNode subDoc = pManager->internalDocument().namedItem("xbel").cloneNode();
    if (subDoc.isProcessingInstruction())
        subDoc = subDoc.nextSibling();
    if (subDoc.isDocumentType())
        subDoc = subDoc.nextSibling();
    if (subDoc.nodeName() != "xbel")
        return;

    if (!folder().isEmpty()) {
        // transform into folder
        subDoc.toElement().setTagName("folder");

        // clear attributes
        QStringList tags;
        for (int i = 0; i < subDoc.attributes().count(); i++)
            tags << subDoc.attributes().item(i).toAttr().name();
        for (QStringList::const_iterator it = tags.constBegin(); it != tags.constEnd(); ++it)
            subDoc.attributes().removeNamedItem((*it));

        subDoc.toElement().setAttribute("icon", m_icon);

        // give the folder a name
        QDomElement textElem = doc.createElement("title");
        subDoc.insertBefore(textElem, subDoc.firstChild());
        textElem.appendChild(doc.createTextNode(folder()));
    }

    // import and add it
    QDomNode node = doc.importNode(subDoc, true);

    if (!folder().isEmpty()) {
        GlobalBookmarkManager::self()->root().internalElement().appendChild(node);
        m_group = KBookmarkGroup(node.toElement()).address();

    } else {
        QDomElement root = GlobalBookmarkManager::self()->root().internalElement();

        QList<QDomElement> childList;

        QDomNode n = subDoc.firstChild().toElement();
        while (!n.isNull()) {
            QDomElement e = n.toElement();
            if (!e.isNull())
                childList.append(e);
            n = n.nextSibling();
        }

        QList<QDomElement>::Iterator it = childList.begin();
        QList<QDomElement>::Iterator end = childList.end();
        for (; it!= end ; ++it)
            root.appendChild((*it));
    }
}

#include "moc_importers.cpp"
