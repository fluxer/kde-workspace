/*
 *   Copyright © 2008 Fredrik Höglund <fredrik@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

namespace Plasma {
    class Applet;
    class FrameSvg;
    class DialogShadows;
}

#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsScene>


class Dialog : public QWidget
{
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

    void setGraphicsWidget(QGraphicsWidget *widget);
    void show(Plasma::Applet *applet);

protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Plasma::DialogShadows *m_dialogshadows;
    Plasma::FrameSvg *m_background;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QGraphicsWidget *m_widget;
};

#endif

