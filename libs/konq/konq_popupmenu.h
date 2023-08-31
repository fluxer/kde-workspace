/* This file is part of the KDE project
   Copyright (C) 1998-2008 David Faure <faure@kde.org>
   Copyright (C) 2001 Holger Freyther <freyther@yahoo.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KONQPOPUPMENU_H
#define KONQPOPUPMENU_H

#include <konq_export.h>

#include <QMap>
#include <kmenu.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kfileitem.h>
#include <kservice.h>

class KNewFileMenu;
class KFileItemActions;
class KBookmarkManager;
class KonqPopupMenuPrivate;

/**
 * This class implements the popup menu for URLs in konqueror and kdesktop
 * It's usage is very simple : on right click, create the KonqPopupMenu instance
 * with the correct arguments, then exec() to make it appear, then destroy it.
 *
 * Users of KonqPopupMenu include: konqueror, the media applet, the trash applet
 */
class KONQ_EXPORT KonqPopupMenu : public KMenu
{
  Q_OBJECT
public:
    /**
     * Flags set by the calling application
     */
    enum PopupFlag {
        DefaultPopupItems = 0x0000,    /**< default value, no additional menu item */
        ShowNavigationItems = 0x0001,  /**< show "back" and "forward" (usually done when clicking the background of the view, but not an item) */
        ShowUp = 0x0002,               /**< show "up" (same thing, but not over e.g. HTTP). Requires ShowNavigationItems. */
        ShowReload = 0x0004,           /**< show "reload" (usually done when clicking the background of the view, but not an item) */
        ShowBookmark = 0x0008,         /**< show "add to bookmarks" (usually not done on the local filesystem) */
        ShowCreateDirectory = 0x0010,  /**< show "create directory" (usually only done on the background of the view, or
                                        *   in hierarchical views like directory trees, where the new dir would be visible) */
        ShowTextSelectionItems=0x0020, /**< set when selecting text, for a popup that only contains text-related items. */
        NoDeletion=0x0040,             /**< deletion, trashing and renaming not allowed (e.g. parent dir not writeable).
                                        *  (this is only needed if the protocol itself supports deletion, unlike e.g. HTTP) */
        IsLink = 0x0080,               /**< show "Bookmark This Link" and other link-related actions (linkactions merging group) */
        ShowUrlOperations = 0x0100,    /**< show copy, paste, as well as cut if NoDeletion is not set. */
        ShowProperties = 0x200,        /**< show "Properties" action (usually done by directory views) */
        ShowNewWindow = 0x400,
        NoPlugins = 0x800              /**< for the unittest*/
    };
    Q_DECLARE_FLAGS(PopupFlags, PopupFlag)

    /**
     * Associates a list of actions with a predefined name known by the host's popupmenu:
     * "editactions" for actions related text editing,
     * "linkactions" for actions related to hyperlinks,
     * "partactions" for any other actions provided by the part
     */
    typedef QMap<QString, QList<QAction*>> ActionGroupMap;

    /**
     * Constructor
     * @param manager the bookmark manager for the "add to bookmark" action
     * Only used if KonqPopupMenu::ShowBookmark is set
     * @param items the list of file items the popupmenu should be shown for
     * @param viewURL the URL shown in the view, to test for RMB click on view background
     * @param actions list of actions the caller wants to see in the menu
     * @param newMenu "New" menu, shared with the File menu, in konqueror
     * @param parentWidget the widget we're showing this popup for. Helps destroying
     * the popup if the widget is destroyed before the popup.
     * @param popupFlags flags from the KonqPopupMenu::PopupFlags enum, set by the calling application
     *
     * The actions to pass in include :
     * showmenubar, go_back, go_forward, go_up, cut, copy, paste, pasteto
     * The others items are automatically inserted.
     *
     * @todo that list is probably not be up-to-date
     */
    KonqPopupMenu(const KFileItemList &items,
                  const KUrl& viewURL,
                  KActionCollection & actions,
                  KNewFileMenu * newMenu,
                  PopupFlags popupFlags /*= NoFlags*/,
                  QWidget *parentWidget,
                  KBookmarkManager *manager = 0,
                  const ActionGroupMap& actionGroups = ActionGroupMap());

    /**
     * Don't forget to destroy the object
     */
    ~KonqPopupMenu();

    /**
     * Set the title of the URL, when the popupmenu is opened for a single URL.
     * This is used if the user chooses to add a bookmark for this URL.
     */
    void setURLTitle(const QString &urlTitle);
    KFileItemActions* fileItemActions() const;

private:
    Q_PRIVATE_SLOT(d, void slotPopupNewDir())
    Q_PRIVATE_SLOT(d, void slotPopupNewView())
    Q_PRIVATE_SLOT(d, void slotPopupEmptyTrashBin())
    Q_PRIVATE_SLOT(d, void slotConfigTrashBin())
    Q_PRIVATE_SLOT(d, void slotPopupRestoreTrashedItems())
    Q_PRIVATE_SLOT(d, void slotPopupAddToBookmark())
    Q_PRIVATE_SLOT(d, void slotPopupMimeType())
    Q_PRIVATE_SLOT(d, void slotPopupProperties())
    Q_PRIVATE_SLOT(d, void slotOpenShareFileDialog())
    Q_PRIVATE_SLOT(d, void slotShowOriginalFile())

private:
    KonqPopupMenuPrivate *d;
};

#endif
