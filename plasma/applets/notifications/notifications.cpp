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

#include <Plasma/TabBar>
#include <Plasma/ScrollWidget>
#include <KIcon>
#include <KDebug>

static const QSizeF s_minimumsize = QSizeF(290, 140);

class NotificationsWidget : public Plasma::TabBar
{
    Q_OBJECT
public:
    NotificationsWidget(NotificationsApplet* notifications);
    ~NotificationsWidget();

private Q_SLOTS:
    void slotCountChanged();

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
    m_jobsscrollwidget->setWidget(m_jobswidget);
    addTab(KIcon("services"), i18n("Jobs"), m_jobsscrollwidget);

    m_applicationsscrollwidget = new Plasma::ScrollWidget(this);
    m_applicationsscrollwidget->setMinimumSize(s_minimumsize);
    m_applicationswidget = new ApplicationsWidget(m_applicationsscrollwidget, this);
    connect(
        m_applicationswidget, SIGNAL(countChanged()),
        this, SLOT(slotCountChanged())
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
        m_notifications->setStatus(Plasma::ItemStatus::ActiveStatus);
    } else {
        m_notifications->setStatus(Plasma::ItemStatus::PassiveStatus);
    }
}


NotificationsApplet::NotificationsApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_notificationswidget(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_notifications");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    m_notificationswidget = new NotificationsWidget(this);
    setPopupIcon("preferences-desktop-notification");
}

NotificationsApplet::~NotificationsApplet()
{
    delete m_notificationswidget;
}

QGraphicsWidget* NotificationsApplet::graphicsWidget()
{
    return m_notificationswidget;
}

K_EXPORT_PLASMA_APPLET(notifications, NotificationsApplet)

#include "moc_notifications.cpp"
#include "notifications.moc"
