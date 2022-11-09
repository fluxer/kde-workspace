/*
*   Copyright 2011 by Aaron Seigo <aseigo@kde.org>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License version 2, 
*   or (at your option) any later version.
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

#include "panelshadows.h"

#include <QWidget>
#include <QPainter>

#ifdef Q_WS_X11
#include <QtGui/qx11info_x11.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <fixx11h.h>
#endif

#include <KDebug>
#include <KPixmap>

class PanelShadows::Private
{
public:
    Private(PanelShadows *shadows)
        : q(shadows),
          m_managePixmaps(false),
          top(0),
          right(0),
          bottom(0),
          left(0)
    {
    }

    ~Private()
    {
        clearPixmaps();
    }

    void clearPixmaps();
    void setupPixmaps();
    void initPixmap(const QString &element);
    void updateShadow(const QWidget *window);
    void clearShadow(const QWidget *window);
    void updateShadows();
    void windowDestroyed(QObject *deletedObject);

    PanelShadows *q;
    QList<KPixmap> m_shadowPixmaps;
    QVector<unsigned long> data;
    QSet<const QWidget *> m_windows;
    bool m_managePixmaps;
    int top;
    int right;
    int bottom;
    int left;
};

PanelShadows::PanelShadows(QObject *parent)
    : Plasma::Svg(parent),
      d(new Private(this))
{
    setImagePath("widgets/panel-background");
    connect(this, SIGNAL(repaintNeeded()), this, SLOT(updateShadows()));
}

void PanelShadows::addWindow(const QWidget *window)
{
    if (!window || !window->isWindow()) {
        return;
    }

    d->m_windows << window;
    d->updateShadow(window);
    connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(windowDestroyed(QObject*)));
}

void PanelShadows::removeWindow(const QWidget *window)
{
    if (!d->m_windows.contains(window)) {
        return;
    }

    d->m_windows.remove(window);
    disconnect(window, 0, this, 0);
    d->clearShadow(window);

    if (d->m_windows.isEmpty()) {
        d->clearPixmaps();
    }
}

void PanelShadows::Private::windowDestroyed(QObject *deletedObject)
{
    m_windows.remove(static_cast<QWidget *>(deletedObject));

    if (m_windows.isEmpty()) {
        clearPixmaps();
    }
}

void PanelShadows::Private::updateShadows()
{
    setupPixmaps();
    foreach (const QWidget *window, m_windows) {
        updateShadow(window);
    }
}

void PanelShadows::Private::initPixmap(const QString &element)
{
    m_shadowPixmaps << KPixmap(q->pixmap(element));
}

void PanelShadows::Private::setupPixmaps()
{
    clearPixmaps();
    initPixmap("shadow-top");
    initPixmap("shadow-topright");
    initPixmap("shadow-right");
    initPixmap("shadow-bottomright");
    initPixmap("shadow-bottom");
    initPixmap("shadow-bottomleft");
    initPixmap("shadow-left");
    initPixmap("shadow-topleft");

    foreach (const KPixmap &pixmap, m_shadowPixmaps) {
        data << pixmap.handle();
    }

    QSize marginHint = q->elementSize("shadow-hint-top-margin");
    kDebug() << "top margin hint is:" << marginHint;
    if (marginHint.isValid()) {
        top = marginHint.height();
    } else {
        top = m_shadowPixmaps[0].height(); // top
    }

    marginHint = q->elementSize("shadow-hint-right-margin");
    kDebug() << "right margin hint is:" << marginHint;
    if (marginHint.isValid()) {
        right = marginHint.width();
    } else {
        right = m_shadowPixmaps[2].width(); // right
    }

    marginHint = q->elementSize("shadow-hint-bottom-margin");
    if (marginHint.isValid()) {
        bottom = marginHint.height();
    } else {
        bottom = m_shadowPixmaps[4].height(); // bottom
    }

    marginHint = q->elementSize("shadow-hint-left-margin");
    if (marginHint.isValid()) {
        left = marginHint.width();
    } else {
        left = m_shadowPixmaps[6].width(); // left
    }

    data << top << right << bottom << left;
}

void PanelShadows::getMargins(int &top, int &right, int &bottom, int &left)
{
    top = d->top;
    right = d->right;
    bottom = d->bottom;
    left = d->left;
}

void PanelShadows::Private::clearPixmaps()
{
#warning FIXME: pixmaps are leaked
    if (m_managePixmaps) {
        foreach (KPixmap &pixmap, m_shadowPixmaps) {
            pixmap.release();
        }
        m_managePixmaps = false;
    }
    m_shadowPixmaps.clear();
    data.clear();
}

void PanelShadows::Private::updateShadow(const QWidget *window)
{
#ifdef Q_WS_X11
    if (data.isEmpty()) {
        setupPixmaps();
    }

    Display *dpy = QX11Info::display();
    Atom atom = XInternAtom(dpy, "_KDE_NET_WM_SHADOW", False);

    //kDebug() << "going to set the shadow of" << winId() << "to" << data;
    XChangeProperty(dpy, window->winId(), atom, XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<const unsigned char *>(data.constData()), data.size());
#endif
}

void PanelShadows::Private::clearShadow(const QWidget *window)
{
#ifdef Q_WS_X11
    Display *dpy = QX11Info::display();
    Atom atom = XInternAtom(dpy, "_KDE_NET_WM_SHADOW", False);
    XDeleteProperty(dpy, window->winId(), atom);
#endif
}

#include "moc_panelshadows.cpp"

