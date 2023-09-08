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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QGraphicsSceneMouseEvent>
#include <QComboBox>
#include <QSpacerItem>
#include <KKeyboardLayout>
#include <KConfigDialog>
#include <Plasma/Applet>

class KeyboardApplet : public Plasma::Applet
{
    Q_OBJECT
public:
    KeyboardApplet(QObject *parent, const QVariantList &args);

    // Plasma::Applet reimplementations
    void init() final;
    void paintInterface(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        const QRect &contentsRect);
    void createConfigurationInterface(KConfigDialog *parent) final;
    // QGraphicsWidget reimplementations
    void mousePressEvent(QGraphicsSceneMouseEvent *event) final;
    void wheelEvent(QGraphicsSceneWheelEvent *event) final;

protected:
    // Plasma::Applet reimplementation
    void constraintsEvent(Plasma::Constraints constraints) final;

private Q_SLOTS:
    void slotLayoutChanged();
    void slotNextLayout();
    void slotPreviousLayout();
    void slotConfigAccepted();

private:
    KKeyboardLayout* m_keyboardlayout;
    bool m_showflag;
    bool m_showtext;
    QComboBox* m_indicatorbox;
    QSpacerItem* m_spacer;
};

K_EXPORT_PLASMA_APPLET(keyboard, KeyboardApplet)

#endif // KEYBOARD_H
