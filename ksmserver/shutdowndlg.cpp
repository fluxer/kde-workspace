/*****************************************************************
ksmserver - the KDE session management server

Copyright 2000 Matthias Ettrich <ettrich@kde.org>
Copyright 2007 Urs Wolfer <uwolfer @ kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include "config-workspace.h"
#include "shutdowndlg.h"
#include "plasma/framesvg.h"
#include "plasma/theme.h"

#include <stdio.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QFile>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativePropertyMap>

#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kuser.h>
#include <kjob.h>
#include <Solid/PowerManagement>
#include <kwindowsystem.h>
#include <netwm.h>
#include <KStandardDirs>
#include <kdeclarative.h>
#include <kxerrorhandler.h>

#include <kworkspace/kdisplaymanager.h>

#include "moc_shutdowndlg.cpp"

void KSMShutdownFeedback::start()
{
    if( KWindowSystem::compositingActive()) {
        // Announce that the user MAY be logging out (Intended for the compositor)
        Display* dpy = QX11Info::display();
        Atom announce = XInternAtom(dpy, "_KDE_LOGGING_OUT", False);
        unsigned char dummy = 0;
        XChangeProperty(dpy, QX11Info::appRootWindow(), announce, announce, 8, PropModeReplace, &dummy, 1);
    }
}

void KSMShutdownFeedback::stop()
{
    if( KWindowSystem::compositingActive()) {
        // No longer logging out, announce (Intended for the compositor)
        Display* dpy = QX11Info::display();
        Atom announce = XInternAtom(dpy, "_KDE_LOGGING_OUT", False);
        XDeleteProperty(QX11Info::display(), QX11Info::appRootWindow(), announce);
    }
}

////////////

Q_DECLARE_METATYPE(Solid::PowerManagement::SleepState)

KSMShutdownDlg::KSMShutdownDlg( QWidget* parent,
                                bool maysd, bool choose, KWorkSpace::ShutdownType sdtype,
                                const QString& theme)
  : QDialog( parent, Qt::Popup ) //krazy:exclude=qclasses
    // this is a WType_Popup on purpose. Do not change that! Not
    // having a popup here has severe side effects.
{
    KSMShutdownFeedback::start(); // make the screen gray

    KDialog::centerOnScreen(this, -3);

    //kDebug() << "Creating QML view";
    m_view = new QDeclarativeView(this);
    QDeclarativeContext *context = m_view->rootContext();
    context->setContextProperty("maysd", maysd);
    context->setContextProperty("choose", choose);
    context->setContextProperty("sdtype", sdtype);

    QDeclarativePropertyMap *mapShutdownType = new QDeclarativePropertyMap(this);
    mapShutdownType->insert("ShutdownTypeDefault", QVariant::fromValue((int)KWorkSpace::ShutdownTypeDefault));
    mapShutdownType->insert("ShutdownTypeNone", QVariant::fromValue((int)KWorkSpace::ShutdownTypeNone));
    mapShutdownType->insert("ShutdownTypeReboot", QVariant::fromValue((int)KWorkSpace::ShutdownTypeReboot));
    mapShutdownType->insert("ShutdownTypeHalt", QVariant::fromValue((int)KWorkSpace::ShutdownTypeHalt));
    mapShutdownType->insert("ShutdownTypeLogout", QVariant::fromValue((int)KWorkSpace::ShutdownTypeLogout));
    context->setContextProperty("ShutdownType", mapShutdownType);

    QDeclarativePropertyMap *mapSpdMethods = new QDeclarativePropertyMap(this);
    QSet<Solid::PowerManagement::SleepState> spdMethods = Solid::PowerManagement::supportedSleepStates();
    mapSpdMethods->insert("SuspendState", QVariant::fromValue(spdMethods.contains(Solid::PowerManagement::SuspendState)));
    mapSpdMethods->insert("HibernateState", QVariant::fromValue(spdMethods.contains(Solid::PowerManagement::HibernateState)));
    mapSpdMethods->insert("HybridSuspendState", QVariant::fromValue(spdMethods.contains(Solid::PowerManagement::HybridSuspendState)));
    context->setContextProperty("spdMethods", mapSpdMethods);

    setModal( true );

    // window stuff
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setWindowFlags(Qt::X11BypassWindowManagerHint);
    m_view->setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
    QPalette pal = m_view->palette();
    pal.setColor(backgroundRole(), Qt::transparent);
    m_view->setPalette(pal);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // engine stuff
    KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_view->engine());
    kdeclarative.initialize();
    kdeclarative.setupBindings();
    m_view->installEventFilter(this);

    QString fileName = KStandardDirs::locate("data", QString("ksmserver/themes/%1/main.qml").arg(theme));
    if (QFile::exists(fileName)) {
        //kDebug() << "Using QML theme" << fileName;
        m_view->setSource(QUrl::fromLocalFile(fileName));
    }
    QGraphicsObject *rootObject = m_view->rootObject();
    connect(rootObject, SIGNAL(logoutRequested()), SLOT(slotLogout()));
    connect(rootObject, SIGNAL(haltRequested()), SLOT(slotHalt()));
    connect(rootObject, SIGNAL(suspendRequested(int)), SLOT(slotSuspend(int)) );
    connect(rootObject, SIGNAL(rebootRequested()), SLOT(slotReboot()));
    connect(rootObject, SIGNAL(cancelRequested()), SLOT(slotCancel()));
    connect(rootObject, SIGNAL(lockScreenRequested()), SLOT(slotLockScreen()));
    m_view->show();
    m_view->setFocus();
    adjustSize();
}

bool KSMShutdownDlg::eventFilter ( QObject * watched, QEvent * event )
{
    if (watched == m_view && event->type() == QEvent::Resize) {
        adjustSize();
    }
    return QDialog::eventFilter(watched, event);
}

void KSMShutdownDlg::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent( e );

    if( KWindowSystem::compositingActive()) {
        clearMask();
    } else {
        setMask(m_view->mask());
    }

    KDialog::centerOnScreen(this, -3);
}

void KSMShutdownDlg::slotLogout()
{
    m_shutdownType = KWorkSpace::ShutdownTypeNone;
    accept();
}

void KSMShutdownDlg::slotReboot()
{
    m_shutdownType = KWorkSpace::ShutdownTypeReboot;
    accept();
}

void KSMShutdownDlg::slotLockScreen()
{
    QDBusMessage call = QDBusMessage::createMethodCall("org.freedesktop.ScreenSaver",
                                                       "/ScreenSaver",
                                                       "org.freedesktop.ScreenSaver",
                                                       "Lock");
    QDBusConnection::sessionBus().asyncCall(call);
    reject();
}

void KSMShutdownDlg::slotHalt()
{
    m_shutdownType = KWorkSpace::ShutdownTypeHalt;
    accept();
}

void KSMShutdownDlg::slotSuspend(int spdMethod)
{
    switch (spdMethod) {
        case Solid::PowerManagement::SuspendState:
            Solid::PowerManagement::requestSleep(Solid::PowerManagement::SuspendState);
            break;
        case Solid::PowerManagement::HibernateState:
            Solid::PowerManagement::requestSleep(Solid::PowerManagement::HibernateState);
            break;
        case Solid::PowerManagement::HybridSuspendState:
            Solid::PowerManagement::requestSleep(Solid::PowerManagement::HybridSuspendState);
            break;
    }
    reject();
}

void KSMShutdownDlg::slotCancel()
{
    KSMShutdownFeedback::stop(); // make the screen become normal again
    reject();
}

bool KSMShutdownDlg::confirmShutdown(
        bool maysd, bool choose, KWorkSpace::ShutdownType& sdtype,
        const QString& theme)
{
    KSMShutdownDlg* l = new KSMShutdownDlg(0,
                                           //KSMShutdownFeedback::self(),
                                           maysd, choose, sdtype, theme);
    // NOTE: KWin logout effect expects class hint values to be ksmserver
    XClassHint classHint;
    classHint.res_name = const_cast<char*>("ksmserver");
    classHint.res_class = const_cast<char*>("ksmserver");
    XSetClassHint(QX11Info::display(), l->winId(), &classHint);

    l->setWindowRole("logoutdialog");

    bool result = l->exec();
    sdtype = l->m_shutdownType;

    delete l;

    return result;
}
