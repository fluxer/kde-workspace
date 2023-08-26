/* This file is part of the KDE Project
   Copyright (c) 2005 Jean-Remy Falleri <jr.falleri@laposte.net>
   Copyright (c) 2005-2007 Kevin Ottens <ervin@kde.org>
   Copyright (c) 2007 Alexis Ménard <darktears31@gmail.com>
   Copyright (c) 2011 Lukas Tinkl <ltinkl@redhat.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "soliduiserver.h"

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdesktopfileactions.h>
#include <kpassworddialog.h>
#include <kauthorization.h>
#include <knotification.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <solid/block.h>
#include <solid/storagevolume.h>
#include <solid/storageaccess.h>
#include <solid/storagedrive.h>
#include <solid/solidnamespace.h>

#include "deviceactionsdialog.h"
#include "deviceaction.h"
#include "deviceserviceaction.h"
#include "devicenothingaction.h"

#include <QDir>

K_PLUGIN_FACTORY(SolidUiServerFactory,
                 registerPlugin<SolidUiServer>();
    )
K_EXPORT_PLUGIN(SolidUiServerFactory("soliduiserver"))


SolidUiServer::SolidUiServer(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
}

SolidUiServer::~SolidUiServer()
{
}

void SolidUiServer::showActionsDialog(const QString &udi,
                                      const QStringList &desktopFiles)
{
    if (m_udiToActionsDialog.contains(udi)) {
        DeviceActionsDialog *dialog = m_udiToActionsDialog[udi];
        dialog->activateWindow();
        return;
    }


    QList<DeviceAction*> actions;

    foreach (const QString &desktop, desktopFiles) {
        QString filePath = KStandardDirs::locate("data", "solid/actions/"+desktop);

        QList<KServiceAction> services
            = KDesktopFileActions::userDefinedServices(filePath, true);

        foreach (const KServiceAction &service, services) {
            DeviceServiceAction *action = new DeviceServiceAction();
            action->setService(service);
            actions << action;
        }
    }

    // Only one action, execute directly
    if (actions.size()==1) {
        DeviceAction *action = actions.takeFirst();
        Solid::Device device(udi);
        action->execute(device);
        delete action;
        return;
    }

    actions << new DeviceNothingAction();

    DeviceActionsDialog *dialog = new DeviceActionsDialog();
    dialog->setDevice(Solid::Device(udi));
    dialog->setActions(actions);

    connect(dialog, SIGNAL(finished()),
            this, SLOT(onActionDialogFinished()));

    m_udiToActionsDialog[udi] = dialog;

    // Update user activity timestamp, otherwise the notification dialog will be shown
    // in the background due to focus stealing prevention. Entering a new media can
    // be seen as a kind of user activity after all. It'd be better to update the timestamp
    // as soon as the media is entered, but it apparently takes some time to get here.
    kapp->updateUserTimestamp();

    dialog->show();
}

void SolidUiServer::onActionDialogFinished()
{
    DeviceActionsDialog *dialog = qobject_cast<DeviceActionsDialog*>(sender());

    if (dialog) {
        QString udi = dialog->device().udi();
        m_udiToActionsDialog.remove(udi);
    }
}

int SolidUiServer::mountDevice(const QString &device, const QString &mountpoint, bool readonly)
{
    QVariantMap mountarguments;
    mountarguments.insert("device", device);
    mountarguments.insert("mountpoint", mountpoint);
    mountarguments.insert("readonly", readonly);
    int mountreply = KAuthorization::execute(
        "org.kde.soliduiserver.mountunmounthelper", "mount", mountarguments
    );
    // qDebug() << "mount" << mountreply;

    if (mountreply == KAuthorization::NoError) {
        return int(Solid::ErrorType::NoError);
    }
    KNotification::event("soliduiserver/mounterror");
    if (mountreply == KAuthorization::AuthorizationError) {
        return int(Solid::ErrorType::UnauthorizedOperation);
    }
    return int(Solid::ErrorType::OperationFailed);
}

int SolidUiServer::unmountDevice(const QString &mountpoint)
{
    QVariantMap unmountarguments;
    unmountarguments.insert("mountpoint", mountpoint);
    int unmountreply = KAuthorization::execute(
        "org.kde.soliduiserver.mountunmounthelper", "unmount", unmountarguments
    );
    // qDebug() << "unmount" << unmountreply;

    if (unmountreply == KAuthorization::NoError) {
        return int(Solid::ErrorType::NoError);
    }
    KNotification::event("soliduiserver/mounterror");
    if (unmountreply == KAuthorization::AuthorizationError) {
        return int(Solid::ErrorType::UnauthorizedOperation);
    }
    return int(Solid::ErrorType::OperationFailed);
}

int SolidUiServer::mountUdi(const QString &udi)
{
    Solid::Device device(udi);
    Solid::StorageVolume *storagevolume = device.as<Solid::StorageVolume>();
    Solid::Block *block = device.as<Solid::Block>();
    if (!storagevolume || !block) {
        kWarning() << "invalid storage device" << udi;
        return int(Solid::ErrorType::InvalidOption);
    } else if (storagevolume->fsType() == "zfs_member") {
        // create pool on USB stick, put some bad stuff on it, set mount point to / and watch it
        // unfold when mounted
        kWarning() << "storage device is insecure" << udi;
        return int(Solid::ErrorType::Insecure);
    }

    QString devicenode = block->device();
    const QString deviceuuid(storagevolume->uuid());

    bool didcryptopen = false;
#ifdef Q_OS_LINUX
    const QString dmnode = QLatin1String("/dev/mapper/") + deviceuuid;
    if (storagevolume->usage() == Solid::StorageVolume::Encrypted) {
        KPasswordDialog passworddialog(0, KPasswordDialog::NoFlags);

        passworddialog.setPrompt(i18n("'%1' needs a password to be accessed. Please enter a password.", device.description()));
        passworddialog.setPixmap(KIcon(device.icon()).pixmap(64, 64));
        if (!passworddialog.exec()) {
            return int(Solid::ErrorType::UserCanceled);
        }

        QVariantMap cryptopenarguments;
        cryptopenarguments.insert("device", devicenode);
        cryptopenarguments.insert("name", deviceuuid);
        cryptopenarguments.insert("password", passworddialog.password().toLocal8Bit().toHex());
        int cryptopenreply = KAuthorization::execute(
            "org.kde.soliduiserver.mountunmounthelper", "cryptopen", cryptopenarguments
        );
        // qDebug() << "cryptopen" << cryptopenreply;

        if (cryptopenreply == KAuthorization::AuthorizationError) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (cryptopenreply != KAuthorization::NoError) {
            return int(Solid::ErrorType::OperationFailed);
        }

        didcryptopen = true;
        // NOTE: keep in sync with kdelibs/solid/solid/backends/udev/udevstorageaccess.cpp
        devicenode = dmnode;
    }
#else
#warning crypto storage devices are not supported on this platform
#endif

    // permission denied on /run/mount so.. using base directory that is writable
    const QString mountbase = KGlobal::dirs()->saveLocation("tmp");
    const QString mountpoint = mountbase + QLatin1Char('/') + deviceuuid;
    QDir mountdir(mountbase);
    if (!mountdir.exists(mountpoint) && !mountdir.mkdir(mountpoint)) {
        kWarning() << "could not create" << mountpoint;
        if (didcryptopen) {
            QVariantMap cryptclosearguments;
            cryptclosearguments.insert("name", deviceuuid);
            (void)KAuthorization::execute(
                "org.kde.soliduiserver.mountunmounthelper", "cryptclose", cryptclosearguments
            );
        }
        return int(Solid::ErrorType::OperationFailed);
    }

    const int mountresult = mountDevice(devicenode, mountpoint);
    if (mountresult != int(Solid::ErrorType::NoError) && didcryptopen) {
        QVariantMap cryptclosearguments;
        cryptclosearguments.insert("name", deviceuuid);
        (void)KAuthorization::execute(
            "org.kde.soliduiserver.mountunmounthelper", "cryptclose", cryptclosearguments
        );
    }
    return mountresult;
}

int SolidUiServer::unmountUdi(const QString &udi)
{
    Solid::Device device(udi);
    Solid::StorageAccess *storageaccess = device.as<Solid::StorageAccess>();
    Solid::StorageVolume *storagevolume = device.as<Solid::StorageVolume>();
    if (!storagevolume || !storageaccess) {
        kWarning() << "invalid storage device" << udi;
        KNotification::event("soliduiserver/mounterror");
        return int(Solid::ErrorType::InvalidOption);
    }

    const int unmountresult = unmountDevice(storageaccess->filePath());
    if (unmountresult != int(Solid::ErrorType::NoError)) {
        return unmountresult;
    }

    bool isremovable = false;
    Solid::StorageDrive *storagedrive = device.as<Solid::StorageDrive>();
    if (storagedrive) {
        isremovable = (storagedrive->isHotpluggable() || storagedrive->isRemovable());
    }

    if (storagevolume->usage() == Solid::StorageVolume::Encrypted) {
        QVariantMap cryptclosearguments;
        cryptclosearguments.insert("name", storagevolume->uuid());
        int cryptclosereply = KAuthorization::execute(
            "org.kde.soliduiserver.mountunmounthelper", "cryptclose", cryptclosearguments
        );
        // qDebug() << "cryptclose" << cryptclosereply;

        if (cryptclosereply == KAuthorization::AuthorizationError) {
            KNotification::event("soliduiserver/mounterror");
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (cryptclosereply != KAuthorization::NoError) {
            KNotification::event("soliduiserver/mounterror");
            return int(Solid::ErrorType::OperationFailed);
        }
    }

    if (isremovable) {
        KNotification::event("soliduiserver/safetoremove");
    }
    return int(Solid::ErrorType::NoError);
}

QString SolidUiServer::errorString(const int error)
{
    return Solid::errorString(static_cast<Solid::ErrorType>(error));
}

#include "moc_soliduiserver.cpp"
