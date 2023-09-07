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

#include "devicenotifier.h"

#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGridLayout>
#include <Solid/Device>
#include <Solid/DeviceNotifier>
#include <Plasma/Label>
#include <Plasma/Separator>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Meter>
#include <Plasma/ToolTipManager>
#include <Solid/Device>
#include <Solid/Block>
#include <Solid/Predicate>
#include <Solid/StorageVolume>
#include <Solid/StorageDrive>
#include <Solid/StorageAccess>
#include <Solid/OpticalDrive>
#include <KIcon>
#include <KRun>
#include <KDiskFreeSpaceInfo>
#include <KDebug>

Q_DECLARE_METATYPE(Plasma::IconWidget*)
Q_DECLARE_METATYPE(Plasma::Meter*)

static const int s_freetimeout = 3000; // 3secs

class DeviceNotifierWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    DeviceNotifierWidget(DeviceNotifier* devicenotifier);
    ~DeviceNotifierWidget();

    bool onlyremovable;

public Q_SLOTS:
    void slotUpdateLayout();

private Q_SLOTS:
    void slotCheckFreeSpace();
    void slotCheckEmblem(const bool accessible, const QString &udi);
    void slotIconActivated();
    void slotRemoveActivated();

private:
    DeviceNotifier* m_devicenotifier;
    QGraphicsLinearLayout* m_layout;
    Plasma::Label* m_title;
    QList<Plasma::Frame*> m_frames;
    QTimer* m_freetimer;
    QList<Solid::Device> m_soliddevices;
};

DeviceNotifierWidget::DeviceNotifierWidget(DeviceNotifier* devicenotifier)
    : QGraphicsWidget(devicenotifier),
    onlyremovable(true), // TODO: option for it
    m_devicenotifier(devicenotifier),
    m_layout(nullptr),
    m_title(nullptr)
{
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    m_title = new Plasma::Label(this);
    m_title->setText(i18n("No Devices Available"));
    m_title->setAlignment(Qt::AlignCenter);
    m_layout->addItem(m_title);
    setLayout(m_layout);

    m_freetimer = new QTimer(this);
    m_freetimer->setInterval(s_freetimeout);
    connect(
        m_freetimer, SIGNAL(timeout()),
        this, SLOT(slotCheckFreeSpace())
    );
    connect(
        Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(QString)),
        this, SLOT(slotUpdateLayout())
    );
    connect(
        Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(QString)),
        this, SLOT(slotUpdateLayout())
    );
    connect(
        Solid::DeviceNotifier::instance(), SIGNAL(contentChanged(QString,bool)),
        this, SLOT(slotUpdateLayout())
    );
}

DeviceNotifierWidget::~DeviceNotifierWidget()
{
}

void DeviceNotifierWidget::slotUpdateLayout()
{
    Solid::Predicate solidpredicate(Solid::DeviceInterface::StorageVolume);
    QList<Solid::Device> soliddevices = Solid::Device::listFromQuery(solidpredicate);
    QMutableListIterator<Solid::Device> soliditer(soliddevices);
    while (soliditer.hasNext()) {
        const Solid::Device soliddevice = soliditer.next();
        const Solid::StorageVolume *solidstoragevolume = soliddevice.as<Solid::StorageVolume>();
        if (!solidstoragevolume || solidstoragevolume->isIgnored()) {
            soliditer.remove();
            continue;
        }
        if (onlyremovable) {
            const Solid::StorageDrive *solidstoragedrive = soliddevice.as<Solid::StorageDrive>();
            if (!solidstoragedrive || !solidstoragedrive->isRemovable()) {
                soliditer.remove();
                continue;
            }
        }
        // qDebug() << Q_FUNC_INFO << soliddevice.udi() << solidstoragevolume->usage();
    }

    m_freetimer->stop();
    foreach (Plasma::Frame* frame, m_frames) {
        m_layout->removeItem(frame);
    }
    qDeleteAll(m_frames);
    m_frames.clear();
    // ensures that the widget does not expand after items removal
    adjustSize();

    m_soliddevices = soliddevices;
    if (m_soliddevices.isEmpty()) {
        m_title->show();
        m_devicenotifier->setStatus(Plasma::ItemStatus::PassiveStatus);
        return;
    }

    m_title->hide();
    m_devicenotifier->setStatus(Plasma::ItemStatus::ActiveStatus);
    const int smalliconsize = KIconLoader::global()->currentSize(KIconLoader::Small);
    foreach (const Solid::Device &soliddevice, m_soliddevices) {
        const Solid::StorageAccess *solidstorageaccess = soliddevice.as<Solid::StorageAccess>();
        const Solid::OpticalDrive *solidopticaldrive = soliddevice.as<Solid::OpticalDrive>();
        Plasma::Frame* frame = new Plasma::Frame(this);
        frame->setProperty("_k_udi", soliddevice.udi());
        QGraphicsGridLayout* framelayout = new QGraphicsGridLayout(frame);

        Plasma::IconWidget* iconwidget = new Plasma::IconWidget(frame);
        iconwidget->setOrientation(Qt::Horizontal);
        iconwidget->setIcon(KIcon(soliddevice.icon(), KIconLoader::global(), soliddevice.emblems()));
        iconwidget->setText(soliddevice.description());
        iconwidget->setToolTip(i18n("Click to access this device from other applications."));
        iconwidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
        iconwidget->setProperty("_k_udi", soliddevice.udi());
        connect(
            iconwidget, SIGNAL(activated()),
            this, SLOT(slotIconActivated())
        );
        framelayout->addItem(iconwidget, 0, 0, 1, 1);
        frame->setProperty("_k_iconwidget", QVariant::fromValue(iconwidget));

        Plasma::IconWidget* removewidget = new Plasma::IconWidget(frame);
        removewidget->setMaximumIconSize(QSize(smalliconsize, smalliconsize));
        removewidget->setIcon(KIcon("media-eject"));
        removewidget->setToolTip(
            solidopticaldrive ? i18n("Click to eject this disc.") : i18n("Click to safely remove this device.")
        );
        removewidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
        if (solidopticaldrive) {
            removewidget->setVisible(true);
        } else {
            removewidget->setVisible(solidstorageaccess ? solidstorageaccess->isAccessible() : false);
        }
        removewidget->setProperty("_k_udi", soliddevice.udi());
        connect(
            removewidget, SIGNAL(activated()),
            this, SLOT(slotRemoveActivated())
        );
        framelayout->addItem(removewidget, 0, 1, 1, 1);
        frame->setProperty("_k_removewidget", QVariant::fromValue(removewidget));

        Plasma::Meter* meter = new Plasma::Meter(frame);
        meter->setMeterType(Plasma::Meter::BarMeterHorizontal);
        meter->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
        framelayout->addItem(meter, 1, 0, 1, 2);
        frame->setProperty("_k_meter", QVariant::fromValue(meter));

        frame->setLayout(framelayout);
        m_layout->addItem(frame);
        m_frames.append(frame);

        if (solidstorageaccess) {
            connect(
                solidstorageaccess, SIGNAL(accessibilityChanged(bool,QString)),
                this, SLOT(slotCheckEmblem(bool,QString))
            );
        }
    }

    // the minimum space for 2 items, more or less
    QSizeF minimumsize = m_frames.first()->minimumSize();
    minimumsize.setWidth(minimumsize.width() * 1.5);
    minimumsize.setHeight(minimumsize.height() * 2);
    m_devicenotifier->setMinimumSize(minimumsize);

    slotCheckFreeSpace();
    m_freetimer->start();
}

void DeviceNotifierWidget::slotCheckFreeSpace()
{
    foreach (const Plasma::Frame* frame, m_frames) {
        const QString solidudi = frame->property("_k_udi").toString();
        const Solid::Device soliddevice(solidudi);

        const Solid::StorageAccess *solidstorageaccess = soliddevice.as<Solid::StorageAccess>();
        QString devicepath;
        // using the mountpoint is slightly more reliable
        if (solidstorageaccess) {
            devicepath = solidstorageaccess->filePath();
        }
        if (devicepath.isEmpty()) {
            const Solid::Block *solidblock = soliddevice.as<Solid::Block>();
            if (solidblock) {
                devicepath = solidblock->device();
            }
        }
        if (!devicepath.isEmpty()) {
            const KDiskFreeSpaceInfo kfreespaceinfo = KDiskFreeSpaceInfo::freeSpaceInfo(devicepath);
            Plasma::Meter* meter = qvariant_cast<Plasma::Meter*>(frame->property("_k_meter"));
            meter->setMaximum(qMax(kfreespaceinfo.size() / 1024, KIO::filesize_t(100)));
            meter->setValue(kfreespaceinfo.used() / 1024);
            // qDebug() << Q_FUNC_INFO << solidudi << kfreespaceinfo.size() << kfreespaceinfo.used();
        } else {
            kWarning() << "no mountpoint and no device path for" << solidudi;
        }
    }
}

void DeviceNotifierWidget::slotCheckEmblem(const bool accessible, const QString &udi)
{
    foreach (const Plasma::Frame* frame, m_frames) {
        const QString solidudi = frame->property("_k_udi").toString();
        if (solidudi != udi) {
            continue;
        }
        const Solid::Device soliddevice(solidudi);
        const Solid::OpticalDrive *solidopticaldrive = soliddevice.as<Solid::OpticalDrive>();

        Plasma::IconWidget* iconwidget = qvariant_cast<Plasma::IconWidget*>(frame->property("_k_iconwidget"));
        iconwidget->setIcon(KIcon(soliddevice.icon(), KIconLoader::global(), soliddevice.emblems()));

        Plasma::IconWidget* removewidget = qvariant_cast<Plasma::IconWidget*>(frame->property("_k_removewidget"));
        removewidget->setVisible(accessible || solidopticaldrive);
    }
}

void DeviceNotifierWidget::slotIconActivated()
{
    const Plasma::IconWidget* iconwidget = qobject_cast<Plasma::IconWidget*>(sender());
    const QString solidudi = iconwidget->property("_k_udi").toString();
    Solid::Device soliddevice(solidudi);
    Solid::StorageAccess* solidstorageacces = soliddevice.as<Solid::StorageAccess>();
    if (!solidstorageacces) {
        kWarning() << "not storage access" << solidudi;
        return;
    }
    QString mountpoint = solidstorageacces->filePath();
    if (mountpoint.isEmpty()) {
        solidstorageacces->setup();
        mountpoint = solidstorageacces->filePath();
    }
    if (!mountpoint.isEmpty()) {
        KRun::runUrl(KUrl(mountpoint), "inode/directory", nullptr);
    }
}

void DeviceNotifierWidget::slotRemoveActivated()
{
    const Plasma::IconWidget* removewidget = qobject_cast<Plasma::IconWidget*>(sender());
    const QString solidudi = removewidget->property("_k_udi").toString();
    Solid::Device soliddevice(solidudi);
    Solid::OpticalDrive *solidopticaldrive = soliddevice.as<Solid::OpticalDrive>();
    if (solidopticaldrive) {
        solidopticaldrive->eject();
        return;
    }
    Solid::StorageAccess* solidstorageacces = soliddevice.as<Solid::StorageAccess>();
    if (!solidstorageacces) {
        kWarning() << "not storage access" << solidudi;
        return;
    }
    solidstorageacces->teardown();
}

DeviceNotifier::DeviceNotifier(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_plasmascrollwidget(nullptr),
    m_devicenotifierwidget(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_devicenotifier");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    setStatus(Plasma::ItemStatus::PassiveStatus);
    setPopupIcon("device-notifier");
    setMinimumSize(100, 150);
    setPreferredSize(290, 340);

    m_plasmascrollwidget = new Plasma::ScrollWidget(this);
    m_plasmascrollwidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plasmascrollwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_devicenotifierwidget = new DeviceNotifierWidget(this);
    m_plasmascrollwidget->setWidget(m_devicenotifierwidget);
}

DeviceNotifier::~DeviceNotifier()
{
    delete m_devicenotifierwidget;
}

void DeviceNotifier::init()
{
    KConfigGroup configgroup = config();
    m_devicenotifierwidget->onlyremovable = configgroup.readEntry("showOnlyRemovable", true);
    QTimer::singleShot(500, m_devicenotifierwidget, SLOT(slotUpdateLayout()));
}

QGraphicsWidget* DeviceNotifier::graphicsWidget()
{
    return m_plasmascrollwidget;
}

void DeviceNotifier::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* widgetlayout = new QVBoxLayout(widget);
    m_removablebox = new QCheckBox(widget);
    m_removablebox->setText(i18n("Removable devices only"));
    m_removablebox->setChecked(m_devicenotifierwidget->onlyremovable);
    widgetlayout->addWidget(m_removablebox);
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    widgetlayout->addSpacerItem(m_spacer);
    widget->setLayout(widgetlayout);
    parent->addPage(widget, i18n("Devices"), "device-notifier");

    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
    connect(m_removablebox, SIGNAL(stateChanged(int)), parent, SLOT(settingsModified()));
}

void DeviceNotifier::slotConfigAccepted()
{
    Q_ASSERT(m_removablebox);
    m_devicenotifierwidget->onlyremovable = m_removablebox->isChecked();
    KConfigGroup configgroup = config();
    configgroup.writeEntry("showOnlyRemovable", m_devicenotifierwidget->onlyremovable);
    emit configNeedsSaving();
    QTimer::singleShot(500, m_devicenotifierwidget, SLOT(slotUpdateLayout()));
}

#include "moc_devicenotifier.cpp"
#include "devicenotifier.moc"
