/*
 *   Copyright 2010 Marco Martin <notmart@gmail.com>
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

#include "appletsview.h"
#include "appletmovespacer.h"
#include "applettitlebar.h"
#include "dragcountdown.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsScene>
#include <QtGui/qgraphicssceneevent.h>
#include <QTimer>

#include <KGlobalSettings>

#include <Plasma/Containment>

AppletsView::AppletsView(QGraphicsItem *parent)
    : Plasma::ScrollWidget(parent),
      m_spacer(0),
      m_spacerLayout(0),
      m_spacerIndex(0),
      m_scrollDown(false),
      m_clickDrag(false),
      m_movingApplets(false),
      m_dragTimeout(1000)
{
    m_dragCountdown = new DragCountdown(this);
    setAlignment(Qt::AlignCenter);

    connect(m_dragCountdown, SIGNAL(dragRequested()), this, SLOT(appletDragRequested()));

    setAcceptHoverEvents(true);
    setAcceptDrops(true);
    setZValue(900);
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setSingleShot(false);
    connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(scrollTimeout()));
}

AppletsView::~AppletsView()
{
}

void AppletsView::setAppletsContainer(AppletsContainer *appletsContainer)
{
    m_appletsContainer = appletsContainer;
    setWidget(appletsContainer);
}

AppletsContainer *AppletsView::appletsContainer() const
{
    return m_appletsContainer;
}

void AppletsView::setImmediateDrag(const bool immediate)
{
    m_dragTimeout = immediate?0:1000;
}

bool AppletsView::immediateDrag() const
{
    return m_dragTimeout == 0;
}

bool AppletsView::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (watched != m_appletsContainer && !m_appletsContainer->isAncestorOf(watched)) {
        return Plasma::ScrollWidget::sceneEventFilter(watched, event);
    }

    if (!m_appletsContainer->containment()) {
        return Plasma::ScrollWidget::sceneEventFilter(watched, event);
    }

    if (event->type() == QEvent::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent *me = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (!(me->buttons() & Qt::LeftButton)) {
            return Plasma::ScrollWidget::sceneEventFilter(watched, event);
        }

        bool found = false;

        foreach (Plasma::Applet *applet, m_appletsContainer->containment()->applets()) {

            m_appletMoved.clear();
            if (applet->boundingRect().contains(applet->mapFromScene(me->scenePos()))) {
                m_appletMoved = applet;

                if (applet == m_appletsContainer->currentApplet()) {
                    found = true;
                }
                break;
            }
        }

        if (m_appletMoved) {
            //put the move indicator in the center of the VISIBLE area of the applet
            const QRectF mappedAppleRect(mapFromItem(m_appletMoved.data(), m_appletMoved.data()->boundingRect()).boundingRect().intersected(boundingRect()));

            m_dragCountdown->setPos(mappedAppleRect.center() - QPoint(m_dragCountdown->size().width()/2, m_dragCountdown->size().height()/2));

            if (!found || (watched->isWidget() && qobject_cast<AppletTitleBar *>(static_cast<QGraphicsWidget *>(watched)))) {
                m_dragCountdown->start(m_dragTimeout);
            } else {
                m_appletMoved.clear();
                m_dragCountdown->stop();
            }
        }

    } else if (event->type() == QEvent::GraphicsSceneMouseMove) {
        QGraphicsSceneMouseEvent *me = static_cast<QGraphicsSceneMouseEvent *>(event);

        if (m_movingApplets) {
            manageMouseMoveEvent(me);
            return true;
        }

        if (QPointF(me->buttonDownScenePos(me->button()) - me->scenePos()).manhattanLength() > KGlobalSettings::dndEventDelay()) {
            m_dragCountdown->stop();
        }

        if (!m_appletsContainer->currentApplet() || !m_appletsContainer->currentApplet()->isAncestorOf(watched)) {
            Plasma::ScrollWidget::sceneEventFilter(watched, event);
            event->ignore();
            return true;
        } else if (m_appletsContainer->currentApplet()->isAncestorOf(watched)) {
            return false;
        }
    //don't manage wheel events over the current applet
    } else if (event->type() == QEvent::GraphicsSceneWheel && m_appletsContainer->currentApplet() && m_appletsContainer->currentApplet()->isAncestorOf(watched)) {
        return false;
    } else if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        QGraphicsSceneMouseEvent *me = static_cast<QGraphicsSceneMouseEvent *>(event);

        m_dragCountdown->stop();

        if (m_movingApplets) {
            manageMouseReleaseEvent(me);
            return true;
        }

        if (m_appletsContainer->expandAll()) {
            return Plasma::ScrollWidget::sceneEventFilter(watched, event);
        }

        foreach (Plasma::Applet *applet, m_appletsContainer->containment()->applets()) {
            if (applet->boundingRect().contains(applet->mapFromScene(me->scenePos()))) {

                if (QPointF(me->pos() - me->buttonDownPos(me->button())).manhattanLength() > KGlobalSettings::dndEventDelay()) {
                    return Plasma::ScrollWidget::sceneEventFilter(watched, event);
                }

                m_appletsContainer->setCurrentApplet(applet);

                return Plasma::ScrollWidget::sceneEventFilter(watched, event);
            }
        }

        if (!m_appletsContainer->currentApplet() || !m_appletsContainer->currentApplet()->isAncestorOf(watched)) {
            return Plasma::ScrollWidget::sceneEventFilter(watched, event);
        }
    } else if (event->type() == QEvent::GraphicsSceneHoverMove) {
        QGraphicsSceneHoverEvent *he = static_cast<QGraphicsSceneHoverEvent *>(event);
        manageHoverMoveEvent(he);
    } else if (event->type() == QEvent::GraphicsSceneDragMove) {
        QGraphicsSceneDragDropEvent *de = static_cast<QGraphicsSceneDragDropEvent *>(event);
        dragMoveEvent(de);
    }

    if (watched == m_appletsContainer->currentApplet()) {
        return false;
    } else {
        return Plasma::ScrollWidget::sceneEventFilter(watched, event);
    }
}

void AppletsView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Plasma::ScrollWidget::mouseReleaseEvent(event);
    m_appletsContainer->setCurrentApplet(0);
}

void AppletsView::appletDragRequested()
{
    if (!m_appletMoved) {
        return;
    }

    m_movingApplets = true;
    m_appletsContainer->setCurrentApplet(0);

    showSpacer(m_appletMoved.data()->mapToItem(this, m_appletMoved.data()->boundingRect().center()));

    if (m_spacerLayout) {
        m_spacerLayout->removeItem(m_appletMoved.data());
        m_appletMoved.data()->raise();
    }
    if (m_spacer) {
        m_spacer->setMinimumSize(m_appletMoved.data()->size());
    }
}

void AppletsView::manageHoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_clickDrag) {
        //Cheat and pretend a mousemoveevent is arrived
        QGraphicsSceneMouseEvent me;
        me.setPos(event->pos());
        me.setScenePos(event->scenePos());
        me.setLastScenePos(event->lastScenePos());
        manageMouseMoveEvent(&me);
        return;
    }
}

void AppletsView::manageMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_appletMoved) {
        return;
    }

    QPointF position = mapFromScene(event->scenePos());

    if (m_spacer) {
        QPointF delta = event->scenePos()-event->lastScenePos();
        m_appletMoved.data()->moveBy(delta.x(), delta.y());
        showSpacer(position);

        //put the move indicator in the center of the VISIBLE area of the applet
        const QRectF mappedAppleRect(mapFromItem(m_appletMoved.data(), m_appletMoved.data()->boundingRect()).boundingRect().intersected(boundingRect()));

        m_dragCountdown->setPos(mappedAppleRect.center() - QPoint(m_dragCountdown->size().width()/2, m_dragCountdown->size().height()/2));
    }

    if (m_appletsContainer->orientation() == Qt::Vertical) {
        if (pos().y() + position.y() > size().height()*0.70) {
            m_scrollTimer->start(50);
            m_scrollDown = true;
        } else if (position.y() < size().height()*0.30) {
            m_scrollTimer->start(50);
            m_scrollDown = false;
        } else {
            m_scrollTimer->stop();
        }
    } else {
        if (position.x() > size().width()*0.70) {
            m_scrollTimer->start(50);
            m_scrollDown = true;
        } else if (position.x() < size().width()*0.30) {
            m_scrollTimer->start(50);
            m_scrollDown = false;
        } else {
            m_scrollTimer->stop();
        }
    }

    update();
}

void AppletsView::manageMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_appletMoved) {
        return;
    }

    m_scrollTimer->stop();

    QPointF origin = event->buttonDownScenePos(event->button());
    QPoint delta = event->scenePos().toPoint() - origin.toPoint();
    if (!m_clickDrag && origin != QPointF() && delta.manhattanLength() < KGlobalSettings::dndEventDelay()) {
        m_clickDrag = true;
        setAcceptHoverEvents(true);
        return;
    }

    setAcceptHoverEvents(false);
    m_clickDrag = false;


    m_movingApplets = false;

    if (m_spacer && m_spacerLayout) {
        m_spacerLayout->insertItem(m_spacerIndex, m_appletMoved.data());
        m_spacerLayout->removeItem(m_spacer);
    }

    delete m_spacer;
    m_spacer = 0;
    m_spacerLayout = 0;
    m_spacerIndex = 0;
    m_appletMoved.clear();
}

void AppletsView::showSpacer(const QPointF &pos)
{
    if (!scene()) {
        return;
    }

    QPointF translatedPos = pos - m_appletsContainer->pos();

    QGraphicsLinearLayout *lay = 0;

    for (int i = 0; i < m_appletsContainer->count(); ++i) {
        QGraphicsLinearLayout *candidateLay = dynamic_cast<QGraphicsLinearLayout *>(m_appletsContainer->itemAt(i));

        //normally should never happen
        if (!candidateLay) {
            continue;
        }

        if (m_appletsContainer->orientation() == Qt::Horizontal) {
            if (translatedPos.y() < candidateLay->geometry().bottom()) {
                lay = candidateLay;
                break;
            }
        //vertical
        } else {
            if (translatedPos.x() < candidateLay->geometry().right()) {
                lay = candidateLay;
                break;
            }
        }
    }

    //couldn't decide: is the last column empty?
    if (!lay) {
        QGraphicsLinearLayout *candidateLay = dynamic_cast<QGraphicsLinearLayout *>(m_appletsContainer->itemAt(m_appletsContainer->count()-1));

        if (candidateLay && candidateLay->count() <= 2) {
            lay = candidateLay;
        }
    }

    //give up, make a new column
    if (!lay) {
        lay = m_appletsContainer->addColumn();
    }

    if (pos == QPoint()) {
        if (m_spacer) {
            lay->removeItem(m_spacer);
            m_spacer->hide();
        }
        return;
    }

    //lucky case: the spacer is already in the right position
    if (m_spacer && m_spacer->geometry().contains(translatedPos)) {
        return;
    }

    int insertIndex = -1;

    for (int i = 0; i < lay->count(); ++i) {
        QRectF siblingGeometry = lay->itemAt(i)->geometry();

        if (m_appletsContainer->orientation() == Qt::Horizontal) {
            qreal middle = siblingGeometry.center().x();
            if (translatedPos.x() < middle) {
                insertIndex = i;
                break;
            } else if (translatedPos.x() <= siblingGeometry.right()) {
                insertIndex = i + 1;
                break;
            }
        } else { // Vertical
            qreal middle = siblingGeometry.center().y();

            if (translatedPos.y() < middle) {
                insertIndex = i;
                break;
            } else if (translatedPos.y() <= siblingGeometry.bottom()) {
                insertIndex = i + 1;
                break;
            }
        }
    }

    if (m_spacerLayout == lay && m_spacerIndex < insertIndex) {
        --insertIndex;
    }
    if (lay->count() > 1 && insertIndex >= lay->count() - 1) {
        --insertIndex;
    }

    m_spacerIndex = insertIndex;
    if (insertIndex != -1) {
        if (!m_spacer) {
            m_spacer = new AppletMoveSpacer(this);
            connect (m_spacer, SIGNAL(dropRequested(QGraphicsSceneDragDropEvent *)),
                     this, SLOT(spacerRequestedDrop(QGraphicsSceneDragDropEvent *)));
        }
        if (m_spacerLayout) {
            m_spacerLayout->removeItem(m_spacer);
        }
        m_spacer->show();
        lay->insertItem(insertIndex, m_spacer);
        m_spacerLayout = lay;
    }
}

void AppletsView::scrollTimeout()
{

    if (m_appletsContainer->orientation() == Qt::Vertical) {
        if (m_scrollDown) {
            if (m_appletsContainer->geometry().bottom() > geometry().bottom()) {
                m_appletsContainer->moveBy(0, -10);
                if (m_appletMoved) {
                    m_appletMoved.data()->moveBy(0, 10);
                }
            }
        } else {
            if (m_appletsContainer->pos().y() < 0) {
                m_appletsContainer->moveBy(0, 10);
                if (m_appletMoved) {
                    m_appletMoved.data()->moveBy(0, -10);
                }
            }
        }
    } else {
        if (m_scrollDown) {
            if (m_appletsContainer->geometry().right() > geometry().right()) {
                m_appletsContainer->moveBy(-10, 0);
                if (m_appletMoved) {
                    m_appletMoved.data()->moveBy(10, 0);
                }
            }
        } else {
            if (m_appletsContainer->pos().x() < 0) {
                m_appletsContainer->moveBy(10, 0);
                if (m_appletMoved) {
                    m_appletMoved.data()->moveBy(-10, 0);
                }
            }
        }
    }
}


void AppletsView::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    showSpacer(mapFromScene(event->scenePos()));
    event->accept();
}

void AppletsView::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QPointF pos = mapFromScene(event->scenePos());

    if (pos.y() > size().height()*0.70) {
        m_scrollTimer->start(50);
        m_scrollDown = true;
    } else if (pos.y() < size().height()*0.30) {
        m_scrollTimer->start(50);
        m_scrollDown = false;
    } else {
        m_scrollTimer->stop();
    }

    showSpacer(pos);
}

void AppletsView::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    m_scrollTimer->stop();
}

void AppletsView::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (m_spacerLayout) {
        m_spacerLayout->removeItem(m_spacer);
    }

    if (m_spacer) {
        m_spacer->deleteLater();
    }

    m_scrollTimer->stop();

    m_spacer = 0;
    m_spacerLayout = 0;
    m_spacerIndex = 0;

    emit dropRequested(event);
}

void AppletsView::spacerRequestedDrop(QGraphicsSceneDragDropEvent *event)
{
    event->setPos(mapFromScene(event->scenePos()));
    dropEvent(event);
}

#include "moc_appletsview.cpp"

