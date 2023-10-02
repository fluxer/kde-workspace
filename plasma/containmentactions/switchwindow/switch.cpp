/*
 *   Copyright 2009 by Chani Armitage <chani@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "switch.h"
#include "kworkspace/ktaskmanager.h"

#include <KMenu>
#include <KWindowSystem>
#include <KIcon>
#include <KDebug>

SwitchWindow::SwitchWindow(QObject *parent, const QVariantList &args)
    : Plasma::ContainmentActions(parent, args),
      m_menu(new KMenu()),
      m_action(new QAction(this)),
      m_mode(AllFlat),
      m_clearOrderTimer(0)
{
    m_menu->setTitle(i18n("Windows"));
    connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(switchTo(QAction*)));

    m_action->setMenu(m_menu);
}

SwitchWindow::~SwitchWindow()
{
    delete m_menu;
}

void SwitchWindow::init(const KConfigGroup &config)
{
    m_mode = (MenuMode)config.readEntry("mode", (int)AllFlat);
}

QWidget* SwitchWindow::createConfigurationInterface(QWidget* parent)
{
    QWidget *widget = new QWidget(parent);
    m_ui.setupUi(widget);
    widget->setWindowTitle(i18n("Configure Switch Window Plugin"));
    switch (m_mode) {
        case AllFlat:
            m_ui.flatButton->setChecked(true);
            break;
        case DesktopSubmenus:
            m_ui.subButton->setChecked(true);
            break;
        case CurrentDesktop:
            m_ui.curButton->setChecked(true);
            break;
    }
    return widget;
}

void SwitchWindow::configurationAccepted()
{
    if (m_ui.flatButton->isChecked()) {
        m_mode = AllFlat;
    } else if (m_ui.subButton->isChecked()) {
        m_mode = DesktopSubmenus;
    } else {
        m_mode = CurrentDesktop;
    }
}

void SwitchWindow::save(KConfigGroup &config)
{
    config.writeEntry("mode", (int)m_mode);
}

void SwitchWindow::contextEvent(QEvent *event)
{
    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:
            contextEvent(static_cast<QGraphicsSceneMouseEvent*>(event));
            break;
        case QEvent::GraphicsSceneWheel:
            wheelEvent(static_cast<QGraphicsSceneWheelEvent*>(event));
            break;
        default:
            break;
    }
}

void SwitchWindow::makeMenu()
{
    m_menu->clear();

    QMultiHash<int, QAction*> desktops;

    // make all the window actions
    foreach (const WId task, KTaskManager::self()->tasks()) {
        const KWindowInfo kwindowinfo = KWindowSystem::windowInfo(
            task,
            NET::WMVisibleName | NET::WMDesktop
        );
        const QString taskname = kwindowinfo.visibleName();
        if (taskname.isEmpty()) {
            kDebug() << "skipping task with empty name" << task;
            continue;
        }

        QAction *action = new QAction(taskname, m_menu);
        action->setIcon(KIcon(KWindowSystem::icon(task)));
        action->setData(qlonglong(task));
        desktops.insert(kwindowinfo.desktop(), action);
    }

    //sort into menu
    if (m_mode == CurrentDesktop) {
        int currentDesktop = KWindowSystem::currentDesktop();
        m_menu->addTitle(i18n("Windows"));
        m_menu->addActions(desktops.values(currentDesktop));
        m_menu->addActions(desktops.values(-1));
    } else {
        int numDesktops = KWindowSystem::numberOfDesktops();
        if (m_mode == AllFlat) {
            for (int i = 0; i <= numDesktops; ++i) {
                if (desktops.contains(i)) {
                    QString name = KWindowSystem::desktopName(i);
                    if (name.isEmpty()) {
                        name = QString::number(i);
                    }
                    m_menu->addTitle(name);
                    m_menu->addActions(desktops.values(i));
                }
            }
            if (desktops.contains(-1)) {
                m_menu->addTitle(i18n("All Desktops"));
                m_menu->addActions(desktops.values(-1));
            }
        } else { //submenus
            for (int i = 0; i <= numDesktops; ++i) {
                if (desktops.contains(i)) {
                    QString name = KWindowSystem::desktopName(i);
                    if (name.isEmpty()) {
                        name = QString::number(i);
                    }
                    KMenu *subMenu = new KMenu(name, m_menu);
                    subMenu->addActions(desktops.values(i));
                    m_menu->addMenu(subMenu);
                }
            }
            if (desktops.contains(-1)) {
                KMenu *subMenu = new KMenu(i18n("All Desktops"), m_menu);
                subMenu->addActions(desktops.values(-1));
                m_menu->addMenu(subMenu);
            }
        }
    }

    m_menu->adjustSize();
}

void SwitchWindow::contextEvent(QGraphicsSceneMouseEvent *event)
{
    makeMenu();
    if (!m_menu->isEmpty()) {
        m_menu->exec(popupPosition(m_menu->size(), event));
    }
}

QList<QAction*> SwitchWindow::contextualActions()
{
    makeMenu();
    QList<QAction*> list;
    list << m_action;
    return list;
}

void SwitchWindow::switchTo(QAction *action)
{
    const qlonglong task = action->data().toLongLong();
    kDebug() << "task window" << task;
    KTaskManager::self()->activateRaiseOrIconify(task);
}

void SwitchWindow::clearWindowsOrder()
{
    kDebug() << "CLEARING>.......................";
    m_windowsOrder.clear();
}

void SwitchWindow::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    //TODO somehow find the "next" or "previous" window
    //without changing hte window order (don't want to always go between two windows)
    if (m_windowsOrder.isEmpty()) {
        m_windowsOrder = KWindowSystem::stackingOrder();
    } else {
        if (!m_clearOrderTimer) {
            m_clearOrderTimer = new QTimer(this);
            connect(m_clearOrderTimer, SIGNAL(timeout()), this, SLOT(clearWindowsOrder()));
            m_clearOrderTimer->setSingleShot(true);
            m_clearOrderTimer->setInterval(1000);
        }

        m_clearOrderTimer->start();
    }

    const WId activeWindow = KWindowSystem::activeWindow();
    const bool up = event->delta() > 0;
    bool next = false;
    WId first = 0;
    WId last = 0;
    for (int i = 0; i < m_windowsOrder.count(); ++i) {
        const WId id = m_windowsOrder.at(i);
        const KWindowInfo info(id, NET::WMDesktop | NET::WMVisibleName | NET::WMWindowType);
        if (info.windowType(NET::NormalMask | NET::DialogMask | NET::UtilityMask) != -1 && info.isOnCurrentDesktop()) {
            if (next) {
                KWindowSystem::forceActiveWindow(id);
                return;
            }

            if (first == 0) {
                first = id;
            }

            if (id == activeWindow) {
                if (up) {
                    next = true;
                } else if (last) {
                    KWindowSystem::forceActiveWindow(last);
                    return;
                }
            }

            last = id;
        }
    }

    KWindowSystem::forceActiveWindow(up ? first : last);
}


#include "moc_switch.cpp"
