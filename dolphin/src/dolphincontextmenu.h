/***************************************************************************
 *   Copyright (C) 2006 by Peter Penz <peter.penz19@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef DOLPHINCONTEXTMENU_H
#define DOLPHINCONTEXTMENU_H

#include <KFileItem>
#include <KService>
#include <KUrl>
#include <konq_copytomenu.h>
#include <KMenu>
#include <QObject>
#include <QVector>
#include <QScopedPointer>
#include <QAction>

class DolphinMainWindow;
class KFileItemActions;
class KFileItemListProperties;
class DolphinRemoveAction;

/**
 * @brief Represents the context menu which appears when doing a right
 *        click on an item or the viewport of the file manager.
 *
 * Beside static menu entries (e. g. 'Paste' or 'Properties') two
 * dynamic sub menus are shown when opening a context menu above
 * an item:
 * - 'Open With': Contains all applications which are registered to
 *                open items of the given MIME type.
 * - 'Actions':   Contains all actions which can be applied to the
 *                given item.
 */
class DolphinContextMenu : public KMenu
{
    Q_OBJECT

public:
    enum Command
    {
        None,
        OpenParentFolderInNewWindow,
        OpenParentFolderInNewTab
    };

    /**
     * @parent        Pointer to the main window the context menu
     *                belongs to.
     * @pos           Position in screen coordinates.
     * @fileInfo      Pointer to the file item the context menu
     *                is applied. If 0 is passed, the context menu
     *                is above the viewport.
     * @baseUrl       Base URL of the viewport where the context menu
     *                should be opened.
     */
    DolphinContextMenu(DolphinMainWindow* parent,
                       const QPoint& pos,
                       const KFileItem& fileInfo,
                       const KUrl& baseUrl);

    virtual ~DolphinContextMenu();

    void setCustomActions(const QList<QAction*>& actions);

    /**
     * Opens the context menu model and returns the requested
     * command, that should be triggered by the caller. If
     * Command::None has been returned, either the context-menu
     * had been closed without executing an action or an
     * already available action from the main-window has been
     * executed.
     */
    Command open();

protected:
    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);

private:
    void openTrashContextMenu();
    void openTrashItemContextMenu();
    void openItemContextMenu();
    void openViewportContextMenu();

    void insertDefaultItemActions(const KFileItemListProperties&);

    /**
     * Adds the "Show menubar" action to the menu if the
     * menubar is hidden.
     */
    void addShowMenuBarAction();

    bool placeExists(const KUrl& url) const;

    QAction* createPasteAction();

    KFileItemListProperties& selectedItemsProperties() const;

    /**
     * Returns the file item for m_baseUrl.
     */
    KFileItem baseFileItem();

    /**
     * Adds actions that have been installed as service-menu.
     */
    void addServiceActions(KFileItemActions& fileItemActions);

    /**
     * Adds actions that are provided by a KFileItemActionPlugin.
     */
    void addFileItemPluginActions();

    /**
     * Adds actions that are provided by a KVersionControlPlugin.
     */
    void addVersionControlPluginActions();

    /**
     * Adds custom actions e.g. like the "[x] Expandable Folders"-action
     * provided in the details view.
     */
    void addCustomActions();

private:
    struct Entry
    {
        int type;
        QString name;
        QString filePath;     // empty for separator
        QString templatePath; // same as filePath for template
        QString icon;
        QString comment;
    };

    enum ContextType
    {
        NoContext = 0,
        ItemContext = 1,
        TrashContext = 2
    };

    QPoint m_pos;
    DolphinMainWindow* m_mainWindow;

    KFileItem m_fileInfo;

    KUrl m_baseUrl;
    KFileItem* m_baseFileItem;  /// File item for m_baseUrl

    KFileItemList m_selectedItems;
    mutable KFileItemListProperties* m_selectedItemsProperties;

    int m_context;
    KonqCopyToMenu m_copyToMenu;
    QList<QAction*> m_customActions;

    Command m_command;

    DolphinRemoveAction* m_removeAction; // Action that represents either 'Move To Trash' or 'Delete'
};

#endif
