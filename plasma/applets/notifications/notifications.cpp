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

#include "notifications.h"
#include "jobswidget.h"
#include "applicationswidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <Plasma/TabBar>
#include <Plasma/ScrollWidget>
#include <KIcon>
#include <KDebug>

static const QSizeF s_minimumsize = QSizeF(290, 140);
static const int s_svgiconsize = 256;
static const uint s_popuptimeout = 3000; // 3secs

static QIcon kNotificationIcon(QObject *parent, const bool active)
{
    QIcon result;
    Plasma::Svg plasmasvg(parent);
    plasmasvg.setImagePath("icons/notification");
    plasmasvg.setContainsMultipleImages(true);
    if (plasmasvg.isValid()) {
        QPixmap iconpixmap(s_svgiconsize, s_svgiconsize);
        iconpixmap.fill(Qt::transparent);
        QPainter iconpainter(&iconpixmap);
        plasmasvg.paint(&iconpainter, iconpixmap.rect(), active ? "notification-inactive" : "notification-disabled");
        result = QIcon(iconpixmap);
    } else {
        result = KIcon("preferences-desktop-notification");
    }
    return result;
}

class NotificationsWidget : public Plasma::TabBar
{
    Q_OBJECT
public:
    NotificationsWidget(NotificationsApplet* notifications);
    ~NotificationsWidget();

private Q_SLOTS:
    void slotCountChanged();
    void slotJobPing();
    void slotApplicationPing();

private:
    NotificationsApplet* m_notifications;
    Plasma::ScrollWidget* m_jobsscrollwidget;
    JobsWidget* m_jobswidget;
    Plasma::ScrollWidget* m_applicationsscrollwidget;
    ApplicationsWidget* m_applicationswidget;
    Plasma::Label* m_notificationslabel;
};

NotificationsWidget::NotificationsWidget(NotificationsApplet* notifications)
    : Plasma::TabBar(notifications),
    m_notifications(notifications),
    m_jobswidget(nullptr),
    m_applicationswidget(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(s_minimumsize);

    m_jobsscrollwidget = new Plasma::ScrollWidget(this);
    m_jobsscrollwidget->setMinimumSize(s_minimumsize);
    m_jobswidget = new JobsWidget(m_jobsscrollwidget, this);
    connect(
        m_jobswidget, SIGNAL(countChanged()),
        this, SLOT(slotCountChanged())
    );
    connect(
        m_jobswidget, SIGNAL(ping()),
        this, SLOT(slotJobPing())
    );
    m_jobsscrollwidget->setWidget(m_jobswidget);
    addTab(KIcon("services"), i18n("Jobs"), m_jobsscrollwidget);

    m_applicationsscrollwidget = new Plasma::ScrollWidget(this);
    m_applicationsscrollwidget->setMinimumSize(s_minimumsize);
    m_applicationswidget = new ApplicationsWidget(m_applicationsscrollwidget, this);
    connect(
        m_applicationswidget, SIGNAL(countChanged()),
        this, SLOT(slotCountChanged())
    );
    connect(
        m_applicationswidget, SIGNAL(ping()),
        this, SLOT(slotApplicationPing())
    );
    m_applicationsscrollwidget->setWidget(m_applicationswidget);
    addTab(KIcon("dialog-information"), i18n("Notifications"), m_applicationsscrollwidget);

    m_notifications->setStatus(Plasma::ItemStatus::PassiveStatus);
}

NotificationsWidget::~NotificationsWidget()
{
}

void NotificationsWidget::slotCountChanged()
{
    const int totalcount = (m_jobswidget->count() + m_applicationswidget->count());
    if (totalcount > 0) {
        m_notifications->setPopupIcon(kNotificationIcon(m_notifications, true));
        m_notifications->setStatus(Plasma::ItemStatus::ActiveStatus);
    } else {
        m_notifications->setPopupIcon(kNotificationIcon(m_notifications, false));
        m_notifications->setStatus(Plasma::ItemStatus::PassiveStatus);
    }
}

void NotificationsWidget::slotJobPing()
{
    // if the popup was shown before the signal it is probably because it is being interacted with
    // so no automatic tab switching in that case
    const bool waspopupshowing = m_notifications->isPopupShowing();
    m_notifications->showPopup(s_popuptimeout);
    if (!waspopupshowing) {
        setCurrentIndex(0);
    }
}

void NotificationsWidget::slotApplicationPing()
{
    const bool waspopupshowing = m_notifications->isPopupShowing();
    m_notifications->showPopup(s_popuptimeout);
    if (!waspopupshowing) {
        setCurrentIndex(1);
    }
}


NotificationsApplet::NotificationsApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_notificationswidget(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_notifications");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setPassivePopup(true);
    m_notificationswidget = new NotificationsWidget(this);
    setPopupIcon(kNotificationIcon(this, false));
}

NotificationsApplet::~NotificationsApplet()
{
    delete m_notificationswidget;
}

QGraphicsWidget* NotificationsApplet::graphicsWidget()
{
    return m_notificationswidget;
}

void NotificationsApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint) {
        switch (formFactor()) {
            case Plasma::FormFactor::Horizontal:
            case Plasma::FormFactor::Vertical: {
                // HACK: limit the widget size to 2-times less than that of the desktop because
                // Plasma::TabBar sets its maximum size to QWIDGETSIZE_MAX which is more than what
                // can fit on panel and for some reason hints do not have effect on the widget size
                // when it is in a panel, see:
                // kdelibs/plasma/widgets/tabbar.cpp
                const QSize desktopsize = qApp->desktop()->size();
                m_notificationswidget->setMaximumSize(desktopsize / 2);
                break;
            }
            default: {
                // back to the Plasma::TabBar maximum on form factor switch
                m_notificationswidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                break;
            }
        }
    }
}

K_EXPORT_PLASMA_APPLET(notifications, NotificationsApplet)

#include "moc_notifications.cpp"
#include "notifications.moc"
