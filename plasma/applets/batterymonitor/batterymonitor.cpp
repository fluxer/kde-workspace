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

#include "batterymonitor.h"

#include <QTimer>
#include <QGraphicsLinearLayout>
#include <Solid/Device>
#include <Solid/Battery>
#include <Solid/PowerManagement>
#include <Solid/DeviceNotifier>
#include <Plasma/CheckBox>
#include <Plasma/Separator>
#include <Plasma/IconWidget>
#include <Plasma/ToolTipManager>
#include <KIcon>
#include <KIconLoader>
#include <KDebug>

static QString kChargeStateToString(const Solid::Battery::ChargeState state)
{
    switch (state) {
        case Solid::Battery::UnknownCharge: {
            return i18n("Unknown");
        }
        case Solid::Battery::Charging: {
            return i18n("Charging");
        }
        case Solid::Battery::Discharging: {
            return i18n("Discharging");
        }
        case Solid::Battery::FullyCharged: {
            return i18n("Fully Charged");
        }
    }
    Q_ASSERT(false);
    return QString();
}

class BatteryMonitorWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    BatteryMonitorWidget(BatteryMonitor* batterymonitor);
    ~BatteryMonitorWidget();

    QString activeBattery() const;
    void setActiveBattery(const QString &udi);

public Q_SLOTS:
    void slotUpdateLayout();

private Q_SLOTS:
    void slotSuppressSleep(const bool suppress);
    void slotSuppressScreen(const bool suppress);
    void slotUpdateActive();
    void slotUpdateIcon(const int state,const QString &udi);

private:
    BatteryMonitor* m_batterymonitor;
    QGraphicsLinearLayout* m_layout;
    int m_suppresssleepcookie;
    Plasma::CheckBox* m_suppresssleepbox;
    int m_suppressscreencookie;
    Plasma::CheckBox* m_suppressscreenbox;
    Plasma::Separator* m_separator;
    QList<Plasma::IconWidget*> m_iconwidgets;
    QString m_activebattery;
};

BatteryMonitorWidget::BatteryMonitorWidget(BatteryMonitor* batterymonitor)
    : QGraphicsWidget(batterymonitor),
    m_batterymonitor(batterymonitor),
    m_layout(nullptr),
    m_suppresssleepcookie(0),
    m_suppresssleepbox(nullptr),
    m_suppressscreencookie(0),
    m_suppressscreenbox(nullptr),
    m_separator(nullptr)
{
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    m_suppresssleepbox = new Plasma::CheckBox(this);
    m_suppresssleepbox->setText(i18n("Suppress sleep power management"));
    connect(m_suppresssleepbox, SIGNAL(toggled(bool)), this, SLOT(slotSuppressSleep(bool)));
    m_layout->addItem(m_suppresssleepbox);
    m_suppressscreenbox = new Plasma::CheckBox(this);
    m_suppressscreenbox->setText(i18n("Suppress screen power management"));
    connect(m_suppressscreenbox, SIGNAL(toggled(bool)), this, SLOT(slotSuppressScreen(bool)));
    m_layout->addItem(m_suppressscreenbox);
    setLayout(m_layout);

    // TODO: selective update
    connect(
        Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(QString)),
        this, SLOT(slotUpdateLayout())
    );
    connect(
        Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(QString)),
        this, SLOT(slotUpdateLayout())
    );
}

BatteryMonitorWidget::~BatteryMonitorWidget()
{
    // have to stop sleep and screen power management supression at some point
    slotSuppressSleep(false);
    slotSuppressScreen(false);
}

QString BatteryMonitorWidget::activeBattery() const
{
    return m_activebattery;
}

void BatteryMonitorWidget::setActiveBattery(const QString &udi)
{
    m_activebattery = udi;
    const Solid::Device soliddevice(udi);
    const Solid::Battery* batterydevice = soliddevice.as<Solid::Battery>();
    if (batterydevice) {
        m_batterymonitor->setPopupIcon(soliddevice.icon());

        QString batterytooltip;
        batterytooltip.append(i18n("Charge percent: %1%<br/>").arg(batterydevice->chargePercent()));
        batterytooltip.append(i18n("Charge state: %1").arg(kChargeStateToString(batterydevice->chargeState())));
        Plasma::ToolTipContent plasmatooltipcontent(soliddevice.description(), batterytooltip, KIcon(soliddevice.icon()));
        Plasma::ToolTipManager::self()->setContent(m_batterymonitor, plasmatooltipcontent);

        if (batterydevice->chargeState() == Solid::Battery::Discharging && batterydevice->chargePercent() < 30) {
            // low battery warning
            m_batterymonitor->setStatus(Plasma::ItemStatus::NeedsAttentionStatus);
        }

        emit m_batterymonitor->configNeedsSaving();
    }
}

void BatteryMonitorWidget::slotUpdateLayout()
{
    foreach (Plasma::IconWidget* iconwidget, m_iconwidgets) {
        m_layout->removeItem(iconwidget);
    }
    qDeleteAll(m_iconwidgets);
    m_iconwidgets.clear();
    if (m_separator) {
        m_layout->removeItem(m_separator);
        delete m_separator;
        m_separator = nullptr;
    }

    QList<Solid::Device> batterydevices = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    if (batterydevices.size() > 0) {
        m_separator = new Plasma::Separator(this);
        m_separator->setOrientation(Qt::Horizontal);
        m_layout->addItem(m_separator);
        m_batterymonitor->setStatus(Plasma::ItemStatus::ActiveStatus);
    } else {
        m_batterymonitor->setStatus(Plasma::ItemStatus::PassiveStatus);
    }

    const int paneliconsize = KIconLoader::global()->currentSize(KIconLoader::Panel);
    foreach (const Solid::Device &batterydevice, batterydevices) {
        if (m_activebattery.isEmpty()) {
            // pick the first as active
            m_activebattery = batterydevice.udi();
        }

        const Solid::Battery* battery = batterydevice.as<Solid::Battery>();
        Plasma::IconWidget* iconwidget = new Plasma::IconWidget(
            KIcon(batterydevice.icon()), batterydevice.description(),
            this
        );
        iconwidget->setOrientation(Qt::Horizontal);
        iconwidget->setPreferredIconSize(QSize(paneliconsize, paneliconsize));
        iconwidget->setProperty("_k_udi", batterydevice.udi());
        connect(
            iconwidget, SIGNAL(activated()),
            this, SLOT(slotUpdateActive())
        );
        m_iconwidgets.append(iconwidget);
        m_layout->addItem(iconwidget);
        connect(
            battery, SIGNAL(chargePercentChanged(int,QString)),
            this, SLOT(slotUpdateIcon(int,QString))
        );
        connect(
            battery, SIGNAL(chargeStateChanged(int,QString)),
            this, SLOT(slotUpdateIcon(int,QString))
        );
    }

    if (!m_activebattery.isEmpty()) {
        setActiveBattery(m_activebattery);
    }
}

void BatteryMonitorWidget::slotSuppressSleep(const bool suppress)
{
    if (suppress) {
        m_suppresssleepcookie = Solid::PowerManagement::beginSuppressingSleep(QString::fromLatin1("battery monitor"));
        if (m_suppresssleepcookie <= 0) {
            kWarning() << "could not suppress sleep";
            m_batterymonitor->showMessage(
                KIcon("dialog-warning"),
                i18n("Could not suppress sleep power management"),
                Plasma::ButtonOk
            );
            m_suppresssleepbox->setChecked(false);
        }
    } else if (m_suppresssleepcookie > 0) {
        const bool suppressresult = Solid::PowerManagement::stopSuppressingSleep(m_suppresssleepcookie);
        if (!suppressresult) {
            kWarning() << "could not stop sleep suppress";
            m_suppresssleepbox->setChecked(true);
        } else {
            m_suppresssleepcookie = 0;
        }
    }
}

void BatteryMonitorWidget::slotSuppressScreen(const bool suppress)
{
    if (suppress) {
        m_suppressscreencookie = Solid::PowerManagement::beginSuppressingScreenPowerManagement(
            QString::fromLatin1("battery monitor")
        );
        if (m_suppressscreencookie <= 0) {
            kWarning() << "could not suppress screen";
            m_batterymonitor->showMessage(
                KIcon("dialog-warning"),
                i18n("Could not suppress screen power management"),
                Plasma::ButtonOk
            );
            m_suppressscreenbox->setChecked(false);
        }
    } else if (m_suppressscreencookie > 0) {
        bool suppressresult = Solid::PowerManagement::stopSuppressingScreenPowerManagement(m_suppressscreencookie);
        if (!suppressresult) {
            kWarning() << "could not stop screen suppress";
            m_suppressscreenbox->setChecked(true);
        } else {
            m_suppressscreencookie = 0;
        }
    }
}

void BatteryMonitorWidget::slotUpdateActive()
{
    Plasma::IconWidget* iconwidget = qobject_cast<Plasma::IconWidget*>(sender());
    const QString iconwdigetudi = iconwidget->property("_k_udi").toString();
    Q_ASSERT(!iconwdigetudi.isEmpty());
    setActiveBattery(iconwdigetudi);
    m_batterymonitor->hidePopup();
}

void BatteryMonitorWidget::slotUpdateIcon(const int state,const QString &udi)
{
    Q_UNUSED(state);
    foreach (Plasma::IconWidget* iconwidget, m_iconwidgets) {
        const QString iconwdigetudi = iconwidget->property("_k_udi").toString();
        Q_ASSERT(!iconwdigetudi.isEmpty());
        if (iconwdigetudi == udi) {
            const Solid::Device soliddevice(udi);
            iconwidget->setIcon(KIcon(soliddevice.icon()));
            if (iconwdigetudi == m_activebattery) {
                setActiveBattery(m_activebattery);
            }
        }
    }
}


BatteryMonitor::BatteryMonitor(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_batterywidget(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_battery");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
}

BatteryMonitor::~BatteryMonitor()
{
    delete m_batterywidget;
}

void BatteryMonitor::init()
{
    setPopupIcon("battery");
    m_batterywidget = new BatteryMonitorWidget(this);
    configChanged();
}

QGraphicsWidget* BatteryMonitor::graphicsWidget()
{
    return m_batterywidget;
}

void BatteryMonitor::configChanged()
{
    KConfigGroup configgroup = config();
    const QString activebattery = configgroup.readEntry("activeBattery", QString());
    if (!activebattery.isEmpty()) {
        m_batterywidget->setActiveBattery(activebattery);
    }
}

void BatteryMonitor::saveState(KConfigGroup &group) const
{
    group.writeEntry("activeBattery", m_batterywidget->activeBattery());
    Plasma::PopupApplet::saveState(group);
}

void BatteryMonitor::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::StartupCompletedConstraint) {
        QTimer::singleShot(1500, m_batterywidget, SLOT(slotUpdateLayout()));
    }
}

#include "moc_batterymonitor.cpp"
#include "batterymonitor.moc"
