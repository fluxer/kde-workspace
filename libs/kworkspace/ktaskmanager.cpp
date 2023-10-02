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

#include "ktaskmanager.h"

#include <QMutex>
#include <QX11Info>
#include <KGlobal>
#include <KWindowSystem>
#include <KIconLoader>
#include <KIcon>
#include <KLocale>
#include <KDebug>
#include <netwm.h>

static const int s_nodesktop = -1;

static bool kIsTaskWindow(const WId window)
{
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(window, NET::WMWindowType | NET::WMState);
    NET::WindowType kwindowtype = kwindowinfo.windowType(
        NET::NormalMask | NET::DesktopMask | NET::DockMask |
        NET::ToolbarMask | NET::MenuMask | NET::DialogMask |
        NET::UtilityMask | NET::SplashMask
    );
    if (kwindowtype != NET::Normal && kwindowtype != NET::Unknown &&
        kwindowtype != NET::Dialog && kwindowtype != NET::Utility) {
        return false;
    }
    if (kwindowinfo.state() & NET::SkipTaskbar) {
        return false;
    }
    return true;
}

static void kUpdateTask(KTaskManager::Task &task, const bool force = false)
{
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(
        task.window,
        NET::WMVisibleName | NET::WMDesktop
    );
    if (task.name.isEmpty() || force) {
        task.name = kwindowinfo.visibleName();
    }
    task.desktop = kwindowinfo.desktop();
}

class KTaskManagerPrivate : QObject
{
    Q_OBJECT
public:
    KTaskManagerPrivate(QObject *parent);

    QList<KTaskManager::Task> tasks;

private Q_SLOTS:
    void slotNewWindow(const WId window);
    void slotChangedWindow(const WId window);
    void slotRemovedWindow(const WId window);

private:
    QMutex m_mutex;
};

KTaskManagerPrivate::KTaskManagerPrivate(QObject *parent)
    : QObject(parent)
{
    foreach (const WId window, KWindowSystem::stackingOrder()) {
        if (!kIsTaskWindow(window)) {
            continue;
        }
        kDebug() << "adding task for" << window;
        KTaskManager::Task task;
        task.window = window;
        task.desktop = s_nodesktop;
        kUpdateTask(task);
        tasks.append(task);
        KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent);
        emit ktaskmanager->taskAdded(task);
    }
    connect(
        KWindowSystem::self(), SIGNAL(windowAdded(WId)),
        this, SLOT(slotNewWindow(WId))
    );
    connect(
        KWindowSystem::self(), SIGNAL(windowChanged(WId)),
        this, SLOT(slotChangedWindow(WId))
    );
    connect(
        KWindowSystem::self(), SIGNAL(windowRemoved(WId)),
        this, SLOT(slotRemovedWindow(WId))
    );
}

void KTaskManagerPrivate::slotNewWindow(const WId window)
{
    if (!kIsTaskWindow(window)) {
        return;
    }
    QMutexLocker locker(&m_mutex);
    kDebug() << "new window task for" << window;
    KTaskManager::Task task;
    task.window = window;
    task.desktop = s_nodesktop;
    kUpdateTask(task);
    tasks.append(task);
    KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
    emit ktaskmanager->taskAdded(task);
}

void KTaskManagerPrivate::slotChangedWindow(const WId window)
{
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<KTaskManager::Task> iter(tasks);
    while (iter.hasNext()) {
        KTaskManager::Task &task = iter.next();
        if (task.window == window) {
            kDebug() << "changed window task for" << window;
            kUpdateTask(task, true);
            KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
            emit ktaskmanager->taskChanged(task);
            break;
        }
    }
}

void KTaskManagerPrivate::slotRemovedWindow(const WId window)
{
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<KTaskManager::Task> iter(tasks);
    while (iter.hasNext()) {
        const KTaskManager::Task task = iter.next();
        if (task.window == window) {
            kDebug() << "removed window task for" << window;
            iter.remove();
            KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
            emit ktaskmanager->taskRemoved(task);
            break;
        }
    }
}


K_GLOBAL_STATIC(KTaskManager, globalktaskmanager)

KTaskManager::KTaskManager(QObject *parent)
    : QObject(parent),
    d(nullptr)
{
    d = new KTaskManagerPrivate(this);
}

KTaskManager::~KTaskManager()
{
    delete d;
}

QList<KTaskManager::Task> KTaskManager::tasks() const
{
    return d->tasks;
}

bool KTaskManager::isActive(const KTaskManager::Task &task)
{
    const WId activewindow = KWindowSystem::activeWindow();
    return (task.window == activewindow || KWindowSystem::transientFor(task.window) == activewindow);
}

bool KTaskManager::demandsAttention(const KTaskManager::Task &task)
{
    KWindowInfo kwindowinfo = KWindowSystem::windowInfo(
        task.window,
        NET::WMState | NET::XAWMState
    );
    if (kwindowinfo.hasState(NET::DemandsAttention)) {
        return true;
    }
    kwindowinfo = KWindowSystem::windowInfo(
        KWindowSystem::transientFor(task.window),
        NET::WMState | NET::XAWMState
    );
    return kwindowinfo.hasState(NET::DemandsAttention);
}

void KTaskManager::activateRaiseOrIconify(const KTaskManager::Task &task)
{
    if (isActive(task)) {
        KWindowSystem::minimizeWindow(task.window);
        return;
    }
    KWindowSystem::activateWindow(task.window);
    KWindowSystem::raiseWindow(task.window);
}

KTaskManager* KTaskManager::self()
{
    return globalktaskmanager;
}

#include "ktaskmanager.moc"
