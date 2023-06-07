/***************************************************************************
 *   x11embedpainter.h                                                     *
 *                                                                         *
 *   Copyright (C) 2008 Jason Stubbs <jasonbstubbs@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "x11embedpainter.h"

#include <QtCore/QSet>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimer>

#include <KDebug>


#define MAX_PAINTS_PER_SEC 20
#define MIN_TIME_BETWEEN_PAINTS (1000 / MAX_PAINTS_PER_SEC)


namespace SystemTray
{

class X11EmbedPainter::Private
{
public:
    Private(X11EmbedPainter *parent)
        : q(parent),
          fastPaints(0)
    {
        delayedPaintTimer.setSingleShot(true);
        connect(&delayedPaintTimer, SIGNAL(timeout()), q, SLOT(performUpdates()));
    }

    X11EmbedPainter *q;
    QSet<X11EmbedContainer*> containers;
    QElapsedTimer lastPaintTime;
    QTimer delayedPaintTimer;
    int fastPaints;
};


X11EmbedPainter::X11EmbedPainter()
    : d(new Private(this))
{
}


X11EmbedPainter::~X11EmbedPainter()
{
    delete d;
}


void X11EmbedPainter::updateContainer(X11EmbedContainer *container)
{
    if (d->containers.contains(container)) {
        return;
    }

    d->containers.insert(container);

    connect(container, SIGNAL(destroyed(QObject*)),
            this, SLOT(removeContainer(QObject*)));

    if (!d->delayedPaintTimer.isActive()) {
        const qint64 msecsElapsed = d->lastPaintTime.elapsed();
        if (msecsElapsed > MIN_TIME_BETWEEN_PAINTS) {
            ++d->fastPaints;
            if (d->fastPaints > 2) {
                //kDebug() << "Delaying paint by" << msecsToNextPaint << "msecs";
                d->delayedPaintTimer.start(msecsElapsed - MIN_TIME_BETWEEN_PAINTS);
            } else {
                d->delayedPaintTimer.start(0);
            }
        } else {
            d->fastPaints = 0;
            d->delayedPaintTimer.start(0);
        }
    }
}


void X11EmbedPainter::removeContainer(QObject *container)
{
    d->containers.remove(static_cast<X11EmbedContainer*>(container));
}


void X11EmbedPainter::performUpdates()
{
    QMultiHash<QWidget*, X11EmbedContainer*> containersByParent;

    foreach (X11EmbedContainer *container, d->containers) {
        QWidget *topWidget = container;
        while (topWidget->parentWidget()) {
            topWidget = topWidget->parentWidget();
        }
        containersByParent.insert(topWidget, container);
        container->setUpdatesEnabled(false);
    }

    foreach (QWidget *parent, containersByParent.uniqueKeys()) {
        QList<X11EmbedContainer*> containers = containersByParent.values(parent);
        containersByParent.remove(parent);

        QRegion paintRegion;
        QHash<X11EmbedContainer *, QRect> containerRects;
        foreach (X11EmbedContainer *container, containers) {
            QRect rect = QRect(container->mapTo(parent, QPoint(0, 0)), container->size());
            containerRects.insert(container, rect);
            paintRegion = paintRegion.united(rect);
        }

        QPixmap background = QPixmap(parent->size());
        parent->render(&background, paintRegion.boundingRect().topLeft(), paintRegion);

        foreach (X11EmbedContainer *container, containers) {
            container->setBackgroundPixmap(background.copy(containerRects.value(container)));
        }
    }

    foreach (X11EmbedContainer *container, d->containers) {
        container->setUpdatesEnabled(true);
        disconnect(container, SIGNAL(destroyed(QObject*)),
                   this, SLOT(removeContainer(QObject*)));
    }

    d->containers.clear();
    d->lastPaintTime.start();
}

}

#include "moc_x11embedpainter.cpp"
