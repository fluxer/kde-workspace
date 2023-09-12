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

#include "lockout.h"
#include "kworkspace.h"
#include "kdisplaymanager.h"

#include <QVBoxLayout>
#include <QDBusInterface>
#include <Plasma/Svg>
#include <Plasma/ToolTipManager>
#include <Solid/PowerManagement>
#include <KDebug>

static const bool s_showlock = true;
static const bool s_showswitch = true;
static const bool s_showshutdown = true;
static const bool s_showtoram = true;
static const bool s_showtodisk = true;
static const bool s_showhybrid = true;

static const QString s_screensaver = QString::fromLatin1("org.freedesktop.ScreenSaver");

// TODO: confirmation dialog

LockoutApplet::LockoutApplet(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_layout(nullptr),
    m_lockwidget(nullptr),
    m_switchwidget(nullptr),
    m_shutdownwidget(nullptr),
    m_toramwidget(nullptr),
    m_todiskwidget(nullptr),
    m_hybridwidget(nullptr),
    m_showlock(s_showlock),
    m_showswitch(s_showswitch),
    m_showshutdown(s_showshutdown),
    m_showtoram(s_showtoram),
    m_showtodisk(s_showtodisk),
    m_showhybrid(s_showhybrid),
    m_buttonsmessage(nullptr),
    m_lockbox(nullptr),
    m_switchbox(nullptr),
    m_shutdownbox(nullptr),
    m_torambox(nullptr),
    m_todiskbox(nullptr),
    m_hybridbox(nullptr),
    m_spacer(nullptr),
    m_screensaverwatcher(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_lockout");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setPreferredSize(40, 110);
}

void LockoutApplet::init()
{
    m_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);

    m_lockwidget = new Plasma::IconWidget(this);
    m_lockwidget->setIcon("system-lock-screen");
    m_lockwidget->setToolTip(i18n("Lock the screen"));
    connect(
        m_lockwidget, SIGNAL(activated()),
        this, SLOT(slotLock())
    );
    m_layout->addItem(m_lockwidget);

    m_switchwidget = new Plasma::IconWidget(this);
    m_switchwidget->setIcon("system-switch-user");
    m_switchwidget->setToolTip(i18n("Start a parallel session as a different user"));
    connect(
        m_switchwidget, SIGNAL(activated()),
        this, SLOT(slotSwitch())
    );
    m_layout->addItem(m_switchwidget);

    m_shutdownwidget = new Plasma::IconWidget(this);
    m_shutdownwidget->setIcon("system-shutdown");
    m_shutdownwidget->setToolTip(i18n("Logout, turn off or restart the computer"));
    connect(
        m_shutdownwidget, SIGNAL(activated()),
        this, SLOT(slotShutdown())
    );
    m_layout->addItem(m_shutdownwidget);

    m_toramwidget = new Plasma::IconWidget(this);
    m_toramwidget->setIcon("system-suspend");
    m_toramwidget->setToolTip(i18n("Sleep (suspend to RAM)"));
    connect(
        m_toramwidget, SIGNAL(activated()),
        this, SLOT(slotToRam())
    );
    m_layout->addItem(m_toramwidget);

    m_todiskwidget = new Plasma::IconWidget(this);
    m_todiskwidget->setIcon("system-suspend-hibernate");
    m_todiskwidget->setToolTip(i18n("Hibernate (suspend to disk)"));
    connect(
        m_todiskwidget, SIGNAL(activated()),
        this, SLOT(slotToDisk())
    );
    m_layout->addItem(m_todiskwidget);

    m_hybridwidget = new Plasma::IconWidget(this);
    m_hybridwidget->setIcon("system-suspend");
    m_hybridwidget->setToolTip(i18n("Hybrid Suspend (Suspend to RAM and put the system in sleep mode)"));
    connect(
        m_hybridwidget, SIGNAL(activated()),
        this, SLOT(slotHybrid())
    );
    m_layout->addItem(m_hybridwidget);

    KConfigGroup configgroup = config();
    m_showlock = configgroup.readEntry("showLockButton", s_showlock);
    m_showswitch = configgroup.readEntry("showSwitchButton", s_showswitch);
    m_showshutdown = configgroup.readEntry("showShutdownButton", s_showshutdown);
    m_showtoram = configgroup.readEntry("showToRamButton", s_showtoram);
    m_showtodisk = configgroup.readEntry("showToDiskButton", s_showtodisk);
    m_showhybrid = configgroup.readEntry("showHybridButton", s_showhybrid);

    slotUpdateButtons();

    m_screensaverwatcher = new QDBusServiceWatcher(
        s_screensaver,
        QDBusConnection::sessionBus(),
        QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration
    );
    connect(
        m_screensaverwatcher, SIGNAL(serviceRegistered(QString)),
        this, SLOT(slotScreensaverRegistered(QString))
    );
    connect(
        m_screensaverwatcher, SIGNAL(serviceUnregistered(QString)),
        this, SLOT(slotScreensaverUnregistered(QString))
    );
    connect(
        Solid::PowerManagement::notifier(), SIGNAL(supportedSleepStatesChanged()),
        this, SLOT(slotUpdateButtons())
    );
}

void LockoutApplet::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetlayout = new QVBoxLayout(widget);
    m_buttonsmessage = new KMessageWidget(widget);
    m_buttonsmessage->setCloseButtonVisible(false);
    m_buttonsmessage->setMessageType(KMessageWidget::Information);
    m_buttonsmessage->setWordWrap(true);
    m_buttonsmessage->setText(
        i18n(
            "If a button is not visible that is because what it does <b>is not supported on the current host</b>."
        )
    );
    widgetlayout->addWidget(m_buttonsmessage);
    m_lockbox = new QCheckBox(widget);
    m_lockbox->setText(i18n("Show the “Lock” button"));
    m_lockbox->setChecked(m_showlock);
    widgetlayout->addWidget(m_lockbox);
    m_switchbox = new QCheckBox(widget);
    m_switchbox->setText(i18n("Show the “Switch” button"));
    m_switchbox->setChecked(m_showswitch);
    widgetlayout->addWidget(m_switchbox);
    m_shutdownbox = new QCheckBox(widget);
    m_shutdownbox->setText(i18n("Show the “Shutdown” button"));
    m_shutdownbox->setChecked(m_showshutdown);
    widgetlayout->addWidget(m_shutdownbox);
    m_torambox = new QCheckBox(widget);
    m_torambox->setText(i18n("Show the “Suspend to RAM” button"));
    m_torambox->setChecked(m_showtoram);
    widgetlayout->addWidget(m_torambox);
    m_todiskbox = new QCheckBox(widget);
    m_todiskbox->setText(i18n("Show the “Suspend to Disk” button"));
    m_todiskbox->setChecked(m_showtodisk);
    widgetlayout->addWidget(m_todiskbox);
    m_hybridbox = new QCheckBox(widget);
    m_hybridbox->setText(i18n("Show the “Hybrid Suspend” button"));
    m_hybridbox->setChecked(m_showhybrid);
    widgetlayout->addWidget(m_hybridbox);
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    widgetlayout->addSpacerItem(m_spacer);
    widget->setLayout(widgetlayout);
    // insert-button is 16x16 only
    parent->addPage(widget, i18n("Buttons"), "applications-graphics");

    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
    connect(m_lockbox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(m_switchbox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(m_shutdownbox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(m_torambox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(m_todiskbox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
    connect(m_hybridbox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
}

void LockoutApplet::updateOrientation()
{
    switch (formFactor()) {
        case Plasma::FormFactor::Horizontal: {
            m_layout->setOrientation(Qt::Horizontal);
            m_layout->setSpacing(0);
            return;
        }
        case Plasma::FormFactor::Vertical: {
            m_layout->setOrientation(Qt::Vertical);
            m_layout->setSpacing(0);
            return;
        }
        default: {
            m_layout->setSpacing(4);
            break;
        }
    }

    const QSizeF appletsize = size();
    if (appletsize.width() >= appletsize.height()) {
        m_layout->setOrientation(Qt::Horizontal);
    } else {
        m_layout->setOrientation(Qt::Vertical);
    }
}

void LockoutApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::SizeConstraint || constraints & Plasma::FormFactorConstraint) {
        updateOrientation();
    }
}

void LockoutApplet::slotUpdateButtons()
{
    QDBusInterface screensaver(
        s_screensaver, "/ScreenSaver", s_screensaver,
        QDBusConnection::sessionBus()
    );
    m_lockwidget->setVisible(m_showlock && screensaver.isValid());
    // no signals for these
    KDisplayManager kdisplaymanager;
    m_switchwidget->setVisible(m_showswitch && kdisplaymanager.isSwitchable());
    m_shutdownwidget->setVisible(m_showshutdown && KWorkSpace::canShutDown());

    QSet<Solid::PowerManagement::SleepState> sleepstates = Solid::PowerManagement::supportedSleepStates();
    m_toramwidget->setVisible(m_showtoram && sleepstates.contains(Solid::PowerManagement::SuspendState));
    m_todiskwidget->setVisible(m_showtodisk && sleepstates.contains(Solid::PowerManagement::HibernateState));
    m_hybridwidget->setVisible(m_showhybrid && sleepstates.contains(Solid::PowerManagement::HybridSuspendState));
}

void LockoutApplet::slotScreensaverRegistered(const QString &service)
{
    if (service == s_screensaver) {
        slotUpdateButtons();
    }
}

void LockoutApplet::slotScreensaverUnregistered(const QString &service)
{
    if (service == s_screensaver) {
        slotUpdateButtons();
    }
}

void LockoutApplet::slotLock()
{
    QDBusInterface screensaver(
        s_screensaver, "/ScreenSaver", s_screensaver,
        QDBusConnection::sessionBus()
    );
    if (screensaver.isValid()) {
        screensaver.call("Lock");
    }
}

void LockoutApplet::slotSwitch()
{
    KDisplayManager kdisplaymanager;
    kdisplaymanager.newSession();
}

void LockoutApplet::slotShutdown()
{
    // NOTE: KDisplayManager::shutdown() does not involve the session manager
    KWorkSpace::requestShutDown();
}

void LockoutApplet::slotToRam()
{
    Solid::PowerManagement::requestSleep(Solid::PowerManagement::SuspendState);
}

void LockoutApplet::slotToDisk()
{
    Solid::PowerManagement::requestSleep(Solid::PowerManagement::HibernateState);
}

void LockoutApplet::slotHybrid()
{
    Solid::PowerManagement::requestSleep(Solid::PowerManagement::HybridSuspendState);
}

void LockoutApplet::slotConfigAccepted()
{
    m_showlock = m_lockbox->isChecked();
    m_showswitch = m_switchbox->isChecked();
    m_showshutdown = m_shutdownbox->isChecked();
    m_showtoram = m_torambox->isChecked();
    m_showtodisk = m_todiskbox->isChecked();
    m_showhybrid = m_hybridbox->isChecked();

    slotUpdateButtons();

    KConfigGroup configgroup = config();
    configgroup.writeEntry("showLockButton", m_showlock);
    configgroup.writeEntry("showSwitchButton", m_showswitch);
    configgroup.writeEntry("showShutdownButton", m_showshutdown);
    configgroup.writeEntry("showToRamButton", m_showtoram);
    configgroup.writeEntry("showToDiskButton", m_showtodisk);
    configgroup.writeEntry("showHybridButton", m_showhybrid);

    emit configNeedsSaving();
}

#include "moc_lockout.cpp"
