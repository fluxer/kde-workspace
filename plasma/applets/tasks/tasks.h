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

#ifndef TASKS_H
#define TASKS_H

#include "kworkspace/ktaskmanager.h"

#include <QMutex>
#include <QGraphicsLinearLayout>
#include <Plasma/Applet>

class TasksSvg;

class TasksApplet : public Plasma::Applet
{
    Q_OBJECT
public:
    TasksApplet(QObject *parent, const QVariantList &args);

    // Plasma::Applet reimplementation
    void init() final;

private Q_SLOTS:
    void slotTaskChanged(const KTaskManager::Task &task);
    void slotUpdateLayout();

protected:
    // Plasma::Applet reimplementation
    void constraintsEvent(Plasma::Constraints constraints) final;

private:
    QMutex m_mutex;
    QGraphicsLinearLayout* m_layout;
    QGraphicsWidget* m_spacer;
    QList<TasksSvg*> m_taskssvgs;
};

K_EXPORT_PLASMA_APPLET(tasks, TasksApplet)

#endif // TASKS_H
