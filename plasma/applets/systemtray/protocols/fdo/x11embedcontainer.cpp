/***************************************************************************
 *   x11embedcontainer.cpp                                                 *
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

#include "x11embedcontainer.h"
#include "x11embedpainter.h"
#include "fdoselectionmanager.h"

// KDE
#include <KDebug>
#include <KPixmap>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/qx11info_x11.h>

// Xlib
#include <config-X11.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

#ifdef HAVE_XCOMPOSITE
#  include <X11/extensions/Xcomposite.h>
#endif


namespace SystemTray
{

class X11EmbedContainer::Private
{
public:
    Private(X11EmbedContainer *q)
        : q(q),
          picture(None),
          updatesEnabled(true)
    {
    }

    ~Private()
    {
        if (picture) {
           XRenderFreePicture(QX11Info::display(), picture);
        }
        if (!kpixmap.isNull()) {
           kpixmap.release();
        }
    }

    X11EmbedContainer *q;

    XWindowAttributes attr;
    Picture picture;
    KPixmap kpixmap;
    bool updatesEnabled;
    QImage oldBackgroundImage;
};


X11EmbedContainer::X11EmbedContainer(QWidget *parent)
    : QX11EmbedContainer(parent),
      d(new Private(this))
{
    connect(this, SIGNAL(clientIsEmbedded()),
            this, SLOT(ensureValidSize()));
}


X11EmbedContainer::~X11EmbedContainer()
{
    FdoSelectionManager::manager()->removeDamageWatch(this);
    delete d;
}


void X11EmbedContainer::embedSystemTrayClient(WId clientId)
{
    Display *display = QX11Info::display();

    if (!XGetWindowAttributes(display, clientId, &d->attr)) {
        emit error(QX11EmbedContainer::Unknown);
        return;
    }

    XSetWindowAttributes sAttr;
    sAttr.background_pixel = BlackPixel(display, DefaultScreen(display));
    sAttr.border_pixel = BlackPixel(display, DefaultScreen(display));
    sAttr.colormap = d->attr.colormap;

    WId parentId = parentWidget() ? parentWidget()->winId() : DefaultRootWindow(display);
    Window winId = XCreateWindow(display, parentId, 0, 0, d->attr.width, d->attr.height,
                                 0, d->attr.depth, InputOutput, d->attr.visual,
                                 CWBackPixel | CWBorderPixel | CWColormap, &sAttr);

    XWindowAttributes attr;
    if (!XGetWindowAttributes(display, winId, &attr)) {
        emit error(QX11EmbedContainer::Unknown);
        return;
    }

    create(winId);

#if defined(HAVE_XCOMPOSITE) && defined(HAVE_XFIXES) && defined(HAVE_XDAMAGE)
    XRenderPictFormat *format = XRenderFindVisualFormat(display, d->attr.visual);
    if (format && format->type == PictTypeDirect && format->direct.alphaMask &&
        FdoSelectionManager::manager()->haveComposite())
    {
        // Redirect ARGB windows to offscreen storage so we can composite them ourselves
        XRenderPictureAttributes attr;
        attr.subwindow_mode = IncludeInferiors;

        d->picture = XRenderCreatePicture(display, clientId, format, CPSubwindowMode, &attr);
        XCompositeRedirectSubwindows(display, winId, CompositeRedirectManual);
        FdoSelectionManager::manager()->addDamageWatch(this, clientId);

        //kDebug() << "Embedded client uses an ARGB visual -> compositing.";
    } else {
        //kDebug() << "Embedded client is not using an ARGB visual.";
    }
#endif

    // repeat everything from QX11EmbedContainer's ctor that might be relevant
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
    setEnabled(false);

    XSelectInput(display, winId,
                 KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                 KeymapStateMask |
                 PointerMotionMask |
                 EnterWindowMask | LeaveWindowMask |
                 FocusChangeMask |
                 ExposureMask |
                 StructureNotifyMask |
                 SubstructureNotifyMask);

    XFlush(display);

    embedClient(clientId);

    // FIXME: This checks that the client is still valid. Qt won't pick it up
    // if the client closes before embedding completes. However, what happens
    // if the close happens after this point? Should checks happen on a timer
    // until embedding completes perhaps?
    if (!XGetWindowAttributes(QX11Info::display(), clientId, &d->attr)) {
        emit error(QX11EmbedContainer::Unknown);
        return;
    }
}


void X11EmbedContainer::ensureValidSize()
{
    QSize s = QSize(qBound(minimumSize().width(), width(), maximumSize().width()),
                    qBound(minimumSize().height(), height(), maximumSize().height()));
    resize(s);
}


void X11EmbedContainer::setUpdatesEnabled(bool enabled)
{
    d->updatesEnabled = enabled;
}


void X11EmbedContainer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!d->updatesEnabled) {
        return;
    }

    if (!d->picture) {
        FdoSelectionManager::painter()->updateContainer(this);
        return;
    }

    // Taking a detour via a KPixmap is unfortunately the only way we can get
    // the window contents into Katie's backing store.
    KPixmap kpixmap(size());
    Picture kpixmapicture = XRenderCreatePicture(
        x11Info().display(),
        kpixmap.handle(),
        XRenderFindVisualFormat(x11Info().display(), (Visual *)x11Info().visual()),
        0, 0
    );
    if (!kpixmapicture) {
        // if this happens it may result in incorrect rendering but it should not happen
        kWarning() << "XRenderCreatePicture() failed";
        kpixmap.release();
        FdoSelectionManager::painter()->updateContainer(this);
        return;
    }

    XRenderComposite(
        x11Info().display(), PictOpSrc,
        d->picture, None, kpixmapicture,
        0, 0, 0, 0, 0, 0, width(), height()
    );

    QPainter p(this);
    p.drawImage(0, 0, kpixmap.toImage());

    XRenderFreePicture(x11Info().display(), kpixmapicture);
    kpixmap.release();
}

void X11EmbedContainer::setBackgroundPixmap(const QPixmap &background)
{
    if (!clientWinId()) {
        return;
    }

    // Prevent updating the background-image if possible. Updating can cause a very annoying
    // flicker due to the XClearArea, and thus has to be kept to a minimum
    const QImage image = background.toImage();
    if (d->oldBackgroundImage == image) {
      return;
    }
    d->oldBackgroundImage = image;

    Display* display = QX11Info::display();
    if (!d->kpixmap.isNull()) {
        d->kpixmap.release();
    }
    d->kpixmap = KPixmap(background);
    XSetWindowBackgroundPixmap(display, clientWinId(), d->kpixmap.handle());
    XClearArea(display, clientWinId(), 0, 0, 0, 0, True);
}

}

#include "moc_x11embedcontainer.cpp"
