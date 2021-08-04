/***************************************************************************
 *   Copyright (C) 2008 by Montel Laurent <montel@kde.org>                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA          *
 ***************************************************************************/

#ifndef AUTOSTARTITEM_H
#define AUTOSTARTITEM_H

#include <QtGui/qtreewidget.h>
#include <QObject>
#include <KUrl>

#include <QComboBox>
#include <QTreeWidget>
class Autostart;

class AutoStartItem : public QTreeWidgetItem, public QObject
{
public:
    AutoStartItem( const QString &service, QTreeWidgetItem *parent, Autostart* );
    ~AutoStartItem();

    KUrl fileName() const;

    void setPath(const QString &path);

private:
    KUrl m_fileName;
};

class DesktopStartItem : public AutoStartItem
{
public:
    DesktopStartItem( const QString &service, QTreeWidgetItem *parent, Autostart* );
    ~DesktopStartItem();
};


class ScriptStartItem : public AutoStartItem
{
    Q_OBJECT

public:
    ScriptStartItem( const QString &service, QTreeWidgetItem *parent, Autostart* );
    ~ScriptStartItem();
};

#endif
