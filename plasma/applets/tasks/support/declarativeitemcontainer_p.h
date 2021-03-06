/***************************************************************************
 *   Copyright 2011 Marco Martin <mart@kde.org>                            *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef DECLARATIVEITEMCONTAINER_P
#define DECLARATIVEITEMCONTAINER_P

#include <QDeclarativeItem>
#include <QtGui/qgraphicsitem.h>
#include <QGraphicsWidget>
#include <QtGui/qgraphicssceneevent.h>


class DeclarativeItemContainer : public QGraphicsWidget
{
    Q_OBJECT

public:
    DeclarativeItemContainer(QGraphicsItem *parent = 0);
    ~DeclarativeItemContainer();

    void setDeclarativeItem(QDeclarativeItem *item, bool reparent = true);
    QDeclarativeItem *declarativeItem() const;

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

protected Q_SLOTS:
    void widthChanged();
    void heightChanged();

    void minimumWidthChanged();
    void minimumHeightChanged();
    void maximumWidthChanged();
    void maximumHeightChanged();
    void preferredWidthChanged();
    void preferredHeightChanged();

private:
    QWeakPointer<QDeclarativeItem> m_declarativeItem;
};

#endif
