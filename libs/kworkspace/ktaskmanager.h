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

#ifndef KTASKMANAGER_H
#define KTASKMANAGER_H

#include "kworkspace_export.h"

#include <QObject>
#include <QMenu>

class KTaskManagerPrivate;

class KWORKSPACE_EXPORT KTaskManager : public QObject
{
    Q_OBJECT
public:
    struct Task
    {
        QByteArray id;
        QString name;
        QPixmap icon;
        int desktop;
        WId window;
    };

    KTaskManager(QObject *parent = nullptr);
    ~KTaskManager();

    QList<KTaskManager::Task> tasks() const;
    bool isActive(const KTaskManager::Task &task) const;
    bool demandsAttention(const KTaskManager::Task &task) const;
    void activateRaiseOrIconify(const KTaskManager::Task &task);

    static KTaskManager* self();
    static QMenu* menuForWindow(WId windowid, QWidget *parent);
    static QMenu* menuForTask(const KTaskManager::Task &task, QWidget *parent);

Q_SIGNALS:
    void taskAdded(const KTaskManager::Task &task);
    void taskChanged(const KTaskManager::Task &task);
    void taskRemoved(const KTaskManager::Task &task);

private:
    friend KTaskManagerPrivate;
    Q_DISABLE_COPY(KTaskManager);
    KTaskManagerPrivate* d;
};

Q_DECLARE_METATYPE(KTaskManager::Task);

#endif // KTASKMANAGER_H
