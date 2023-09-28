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
#include <KGlobal>
#include <KStartupInfo>
#include <KConfig>
#include <KConfigGroup>
#include <KWindowSystem>
#include <KIconLoader>
#include <KDebug>

static const WId s_nowindow = -1;
static const int s_nodesktop = -1;
static const int s_iconsize = 32;
static const int s_defaultstartuptimeout = 10; // 10secs

static QByteArray kGetTaskID()
{
    return qRandomUuid();
}

static WId kFindStartupWindow(const KStartupInfoId &startupid)
{
    if (startupid.none()) {
        return s_nowindow;
    }
    foreach (const WId window, KWindowSystem::stackingOrder()) {
        if (KStartupInfo::windowStartupId(window) == startupid.id()) {
            return window;
        }
    }
    return s_nowindow;
}

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
    if (task.window == s_nowindow) {
        return;
    }
    const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(
        task.window,
        NET::WMVisibleName | NET::WMDesktop
    );
    if (task.icon.isNull() || force) {
        task.icon = KWindowSystem::icon(task.window);
    }
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
    void slotNewStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata);
    void slotChangedStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata);
    void slotRemovedStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata);
    void slotNewWindow(const WId window);
    void slotChangedWindow(const WId window);
    void slotRemovedWindow(const WId window);
    void slotActiveWindow(const WId window);

private:
    QMutex m_mutex;
    KStartupInfo* m_startupinfo;
};

KTaskManagerPrivate::KTaskManagerPrivate(QObject *parent)
    : QObject(parent),
    m_startupinfo(nullptr)
{
    m_startupinfo = new KStartupInfo(KStartupInfo::CleanOnCantDetect, this);
    // TODO: startup notification via the taskbar is optional
    KConfig kconfig("klaunchrc");
    KConfigGroup kconfiggroup(&kconfig, "TaskbarButtonSettings");
    m_startupinfo->setTimeout(kconfiggroup.readEntry("Timeout", s_defaultstartuptimeout));

    foreach (const WId window, KWindowSystem::stackingOrder()) {
        if (!kIsTaskWindow(window)) {
            continue;
        }
        kDebug() << "adding task for" << window;
        KTaskManager::Task task;
        task.id = kGetTaskID();
        task.window = window;
        task.desktop = s_nodesktop;
        kUpdateTask(task);
        tasks.append(task);
        KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent);
        emit ktaskmanager->taskAdded(task);
    }

    connect(
        m_startupinfo, SIGNAL(gotNewStartup(KStartupInfoId,KStartupInfoData)),
        this, SLOT(slotNewStartup(KStartupInfoId,KStartupInfoData))
    );
    connect(
        m_startupinfo, SIGNAL(gotStartupChange(KStartupInfoId,KStartupInfoData)),
        this, SLOT(slotChangedStartup(KStartupInfoId,KStartupInfoData))
    );
    connect(
        m_startupinfo, SIGNAL(gotRemoveStartup(KStartupInfoId,KStartupInfoData)),
        this, SLOT(slotRemovedStartup(KStartupInfoId,KStartupInfoData))
    );

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
    connect(
        KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
        this, SLOT(slotActiveWindow(WId))
    );
}

void KTaskManagerPrivate::slotNewStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata)
{
    kDebug() << "new startup task for" << startupid.id();
    QMutexLocker locker(&m_mutex);
    KTaskManager::Task task;
    task.id = kGetTaskID();
    task.icon = startupdata.icon();
    task.name = startupdata.name();
    task.desktop = startupdata.desktop();
    task.window = kFindStartupWindow(startupid);
    task.startupinfo = startupid;
    kUpdateTask(task);
    tasks.append(task);
    KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
    emit ktaskmanager->taskAdded(task);
}

void KTaskManagerPrivate::slotChangedStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata)
{
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<KTaskManager::Task> iter(tasks);
    while (iter.hasNext()) {
        KTaskManager::Task &task = iter.next();
        if (task.startupinfo.id() == startupid.id()) {
            kDebug() << "changed startup task for" << startupid.id();
            // try again
            if (task.name.isEmpty()) {
                task.name = startupdata.name();
            }
            if (task.icon.isNull()) {
                const QString startupicon = startupdata.icon();
                if (!startupicon.isEmpty()) {
                    task.icon = KIconLoader::global()->loadIcon(startupicon, KIconLoader::NoGroup, s_iconsize);
                }
            }
            task.desktop = startupdata.desktop();
            if (task.window == s_nowindow) {
                task.window = kFindStartupWindow(startupid);
            }
            // refersh the startup info
            task.startupinfo = startupid;
            kUpdateTask(task);
            KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
            emit ktaskmanager->taskChanged(task);
            break;
        }
    }
}

void KTaskManagerPrivate::slotRemovedStartup(const KStartupInfoId &startupid, const KStartupInfoData &startupdata)
{
    Q_UNUSED(startupdata);
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<KTaskManager::Task> iter(tasks);
    while (iter.hasNext()) {
        KTaskManager::Task &task = iter.next();
        if (task.startupinfo.id() == startupid.id()) {
            kDebug() << "removed startup task for" << startupid.id() << task.window;
            KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
            task.startupinfo = KStartupInfoId();
            if (task.window == s_nowindow) {
                // no window no task
                iter.remove();
                emit ktaskmanager->taskRemoved(task);
            } else {
                // else emit update
                kUpdateTask(task);
                emit ktaskmanager->taskChanged(task);
            }
            break;
        }
    }
}

void KTaskManagerPrivate::slotNewWindow(const WId window)
{
    if (!kIsTaskWindow(window)) {
        return;
    }
    QMutexLocker locker(&m_mutex);
    KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
    const QByteArray windowstartup = KStartupInfo::windowStartupId(window);
    if (!windowstartup.isEmpty()) {
        QMutableListIterator<KTaskManager::Task> iter(tasks);
        while (iter.hasNext()) {
            KTaskManager::Task &task = iter.next();
            if (task.startupinfo.id() == windowstartup) {
                kDebug() << "matched changed task for" << windowstartup << window;
                // startup change
                task.startupinfo = KStartupInfoId();
                task.window = window;
                kUpdateTask(task);
                emit ktaskmanager->taskChanged(task);
                return;
            }
        }
    }
    kDebug() << "new window task for" << window;
    KTaskManager::Task task;
    task.id = kGetTaskID();
    task.window = window;
    task.desktop = s_nodesktop;
    kUpdateTask(task);
    tasks.append(task);
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

void KTaskManagerPrivate::slotActiveWindow(const WId window)
{
    // the active state is not tracked so it is unknown which window became inactive
    KTaskManager* ktaskmanager = qobject_cast<KTaskManager*>(parent());
    QMutexLocker locker(&m_mutex);
    QMutableListIterator<KTaskManager::Task> iter(tasks);
    while (iter.hasNext()) {
        KTaskManager::Task &task = iter.next();
        kUpdateTask(task);
        emit ktaskmanager->taskChanged(task);
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

bool KTaskManager::isActive(const KTaskManager::Task &task) const
{
    if (task.window == s_nowindow) {
        return false;
    }
    // TODO: transients
    return (task.window == KWindowSystem::activeWindow());
}

bool KTaskManager::demandsAttention(const KTaskManager::Task &task) const
{
    // TODO: including transients
    return false;
}

bool KTaskManager::activateRaiseOrIconify(const KTaskManager::Task &task)
{
    if (task.window == s_nowindow) {
        return false;
    }
    if (isActive(task)) {
        KWindowSystem::minimizeWindow(task.window);
        return true;
    }
    KWindowSystem::activateWindow(task.window);
    KWindowSystem::raiseWindow(task.window);
    return true;
}

KTaskManager* KTaskManager::self()
{
    return globalktaskmanager;
}

QMenu* KTaskManager::menuForTask(WId windowid, QWidget *parent)
{
    // TODO:
    return new QMenu(parent);
}

#include "ktaskmanager.moc"
