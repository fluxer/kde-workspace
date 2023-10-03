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

#ifndef PAGER_H
#define PAGER_H

#include <QMutex>
#include <QAction>
#include <QComboBox>
#include <QSpacerItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneWheelEvent>
#include <KConfigDialog>
#include <KCModuleProxy>
#include <Plasma/Applet>

class PagerSvg;

class PagerApplet : public Plasma::Applet
{
    Q_OBJECT
public:
    enum PagerMode {
        ShowNumber = 0,
        ShowName = 1
    };

    PagerApplet(QObject *parent, const QVariantList &args);

    // Plasma::Applet reimplementations
    void init() final;
    void createConfigurationInterface(KConfigDialog *parent) final;
    QList<QAction*> contextualActions() final;
    // QGraphicsWidget reimplementations
    void wheelEvent(QGraphicsSceneWheelEvent *event) final;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) final;

private Q_SLOTS:
    void slotUpdateLayout();
    void slotAddDesktop();
    void slotRemoveDesktop();
    void slotConfigAccepted();

protected:
    // Plasma::Applet reimplementation
    void constraintsEvent(Plasma::Constraints constraints) final;

private:
    void updatePagers();

    QMutex m_mutex;
    QGraphicsLinearLayout* m_layout;
    QList<PagerSvg*> m_pagersvgs;
    QAction* m_adddesktopaction;
    QAction* m_removedesktopaction;
    QList<QAction*> m_actions;
    PagerApplet::PagerMode m_pagermode;
    QComboBox* m_pagermodebox;
    QSpacerItem* m_spacer;
    KCModuleProxy* m_kcmdesktopproxy;
};

K_EXPORT_PLASMA_APPLET(pager, PagerApplet)

#endif // PAGER_H
