// vim: set ts=4 sts=4 sw=4 et:
/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Kellett <lypanov@kde.org>

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

#ifndef ACTIONSIMPL_H
#define ACTIONSIMPL_H

#include <QtCore/QObject>
class FavIconsItrHolder;
class TestLinkItrHolder;
class CommandHistory;
class KBookmarkModel;

class ActionsImpl : public QObject
{
   Q_OBJECT

public:
   ActionsImpl(QObject* parent, KBookmarkModel* model);
   ~ActionsImpl();
   bool save();

   TestLinkItrHolder* testLinkHolder() { return m_testLinkHolder; }
   FavIconsItrHolder* favIconHolder() { return m_favIconHolder; }

public Q_SLOTS:
   void slotLoad();
   void slotSave();
   void slotSaveAs();
   void slotCut();
   void slotCopy();
   void slotPaste();
   void slotRename();
   void slotChangeURL();
   void slotChangeComment();
   void slotChangeIcon();
   void slotDelete();
   void slotNewFolder();
   void slotNewBookmark();
   void slotInsertSeparator();
   void slotSort();
   void slotOpenLink();
   void slotTestSelection();
   void slotTestAll();
   void slotCancelAllTests();
   void slotUpdateFavIcon();
   void slotRecursiveSort();
   void slotUpdateAllFavIcons();
   void slotCancelFavIconUpdates();
   void slotExpandAll();
   void slotCollapseAll();
   void slotImport();
private:
    CommandHistory* commandHistory();
    KBookmarkModel* m_model;
    TestLinkItrHolder* m_testLinkHolder;
    FavIconsItrHolder* m_favIconHolder;
};

#endif
