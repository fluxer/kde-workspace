/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007 Matt Broadstone <mbroadst@gmail.com>
 *   Copyright (c) 2009 Chani Armitage <chani@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
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

#include "desktopview.h"

#include <QFile>
#include <QtGui/qevent.h>
#include <QCoreApplication>

#include <KMenu>
#include <KRun>
#include <KToggleAction>
#include <KWindowSystem>
#include <NETRootInfo>
#include <KDebug>

#include <Plasma/Applet>
#include <Plasma/Corona>
#include <Plasma/Containment>
#include <Plasma/Svg>
#include <Plasma/Wallpaper>
#include <Plasma/Theme>

#include "desktopcorona.h"
#include "plasmaapp.h"


DesktopView::DesktopView(Plasma::Containment *containment, int id, QWidget *parent)
    : Plasma::View(containment, id, parent),
      m_init(false)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    //setCacheMode(QGraphicsView::CacheNone);

    /*FIXME: Work around for a (maybe) Qt bug:
     * QApplication::focusWidget() can't track focus change in QGraphicsProxyWidget
     *   wrapped normal widget (eg. QLineEdit), if the QGraphicsView has called 
     *   setFocusPolicy(Qt::NoFocus)
     * I've created a bug report to Qt Software.
     * There is also a simple reproduce program in case you're interested in this bug:
     *   ftp://fearee:public@public.sjtu.edu.cn/reproduce.tar.gz
     */

    //setFocusPolicy(Qt::NoFocus);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    checkDesktopAffiliation();

    KWindowSystem::setType(winId(), NET::Desktop);
    lower();

    /*
    const int w = 25;
    QPixmap tile(w * 2, w * 2);
    tile.fill(palette().base().color());
    QPainter pt(&tile);
    QColor color = palette().mid().color();
    color.setAlphaF(.6);
    pt.fillRect(0, 0, w, w, color);
    pt.fillRect(w, w, w, w, color);
    pt.end();
    QBrush b(tile);
    setBackgroundBrush(tile);
    */

    // since Plasma::View has a delayed init we need to
    // put a delay also for this call in order to be sure
    // to have correct information set (e.g. screen())
    if (containment) {
        QRect geom = PlasmaApp::self()->corona()->screenGeometry(containment->screen());
        setGeometry(geom);
    }

    DesktopTracker *tracker = DesktopTracker::self();
    connect(tracker, SIGNAL(screenResized(DesktopTracker::Screen)),
            this, SLOT(screenResized(DesktopTracker::Screen)));
    connect(tracker, SIGNAL(screenMoved(DesktopTracker::Screen)),
            this, SLOT(screenMoved(DesktopTracker::Screen)));
}

DesktopView::~DesktopView()
{
}

void DesktopView::checkDesktopAffiliation()
{
    KWindowSystem::setOnAllDesktops(winId(), true);
}

void DesktopView::screenResized(const DesktopTracker::Screen &s)
{
    if (s.id == screen()) {
        kDebug() << screen();
        adjustSize();
    }
}

void DesktopView::screenMoved(const DesktopTracker::Screen &s)
{
    if (s.id == screen()) {
        kDebug() << screen();
        adjustSize();
    }
}

void DesktopView::adjustSize()
{
    // adapt to screen resolution changes
    QRect geom = PlasmaApp::self()->corona()->screenGeometry(screen());
    kDebug() << "screen" << screen() << "geom" << geom;
    setGeometry(geom);
    if (containment()) {
        containment()->resize(geom.size());
        kDebug() << "Containment's geom after resize" << containment()->geometry();
    }

    kDebug() << "Done" << screen() << geometry();
}

void DesktopView::setContainment(Plasma::Containment *containment)
{
    Plasma::Containment *oldContainment = this->containment();
    if (m_init && containment == oldContainment) {
        //kDebug() << "initialized and containment is the same, aborting";
        return;
    }

    PlasmaApp::self()->prepareContainment(containment);
    m_init = true;

    KConfigGroup viewIds(KGlobal::config(), "ViewIds");
    if (oldContainment) {
        disconnect(oldContainment, SIGNAL(toolBoxVisibilityChanged(bool)), this, SLOT(toolBoxOpened(bool)));
        disconnect(oldContainment, SIGNAL(showAddWidgetsInterface(QPointF)), this, SLOT(showWidgetExplorer()));
        viewIds.deleteEntry(QString::number(oldContainment->id()));
    }

    if (containment) {
        connect(containment, SIGNAL(toolBoxVisibilityChanged(bool)), this, SLOT(toolBoxOpened(bool)));
        connect(containment, SIGNAL(showAddWidgetsInterface(QPointF)), this, SLOT(showWidgetExplorer()));
        viewIds.writeEntry(QString::number(containment->id()), id());
        if (containment->corona()) {
            containment->corona()->requestConfigSync();
        }
    }

    View::setContainment(containment);
}

void DesktopView::toolBoxOpened(bool open)
{
    NETRootInfo info(QX11Info::display(), NET::Supported);
    if (!info.isSupported(NET::WM2ShowingDesktop)) {
        return;
    }

    if (open) {
        connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
                this, SLOT(showDesktopUntoggled(WId)));
    } else {
        disconnect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
                   this, SLOT(showDesktopUntoggled(WId)));
    }

    info.setShowingDesktop(open);
}

void DesktopView::showWidgetExplorer()
{
    Plasma::Containment *c = containment();
    if (c) {
        PlasmaApp::self()->showWidgetExplorer(screen(), c);
    }
}

void DesktopView::showDesktopUntoggled(WId id)
{
    Plasma::Containment *c = containment();
    if (c) {
        c->setToolBoxOpen(false);
    }

    KWindowSystem::activateWindow(id);
}

void DesktopView::screenOwnerChanged(int wasScreen, int isScreen, Plasma::Containment* newContainment)
{
    if (PlasmaApp::isPanelContainment(newContainment)) {
        // we don't care about panel containments changing screens on us
        return;
    }

    /*
    kDebug() << "was:" << wasScreen << "is:" << isScreen << "my screen:" << screen()
             << "containment:" << (QObject *)newContainment << newContainment->activity()
             << "current containment" << (QObject *)containment() 
             << "myself:" << (QObject *)this
             << "containment desktop:" << newContainment->desktop();
    */

    if (containment() == newContainment && wasScreen == screen() && isScreen != wasScreen) {
        //kDebug() << "nulling out containment";
        setContainment(0);
    }

    if (isScreen > -1 && isScreen == screen() ) {
        //kDebug() << "setting new containment";
        setContainment(newContainment);
    }
}

#include "moc_desktopview.cpp"

