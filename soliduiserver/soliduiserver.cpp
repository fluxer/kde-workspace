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
#include <kauthaction.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <solid/block.h>
#include <solid/storagevolume.h>
#include <solid/storageaccess.h>
#include <solid/networkshare.h>
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

int SolidUiServer::mountDevice(const QString &udi)
{
    Solid::Device device(udi);

    Solid::NetworkShare *networkshare = device.as<Solid::NetworkShare>();
    if (networkshare) {
        // qDebug() << Q_FUNC_INFO << udi << networkshare->url();

        QString devicenode;
        QString devicefstype;
        switch (networkshare->type()) {
            case Solid::NetworkShare::Unknown: {
                return int(Solid::ErrorType::MissingDriver);
            }
            case Solid::NetworkShare::Nfs: {
                devicenode = QString::fromLatin1("%1:%2").arg(device.product()).arg(device.vendor());
                devicefstype = QString::fromLatin1("nfs");
                break;
            }
            case Solid::NetworkShare::Cifs: {
                devicenode = QString::fromLatin1("//%1/%2").arg(device.product()).arg(device.vendor());
                devicefstype = QString::fromLatin1("cifs");
                break;
            }
            default: {
                kWarning() << "invalid network share type" << networkshare->type();
                return int(Solid::ErrorType::InvalidOption);
            }
        }
        const QString deviceuuid = device.product();

        // permission denied on /run/mount so.. using base directory that is writable
        const QString mountbase = KGlobal::dirs()->saveLocation("tmp");
        const QString mountpoint = mountbase + QLatin1Char('/') + deviceuuid;
        QDir mountdir(mountbase);
        if (!mountdir.exists(deviceuuid) && !mountdir.mkdir(deviceuuid)) {
            kWarning() << "could not create" << mountpoint;
            return int(Solid::ErrorType::OperationFailed);
        }

        KAuth::Action mountaction("org.kde.soliduiserver.mountunmounthelper.mount");
        mountaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        mountaction.addArgument("device", devicenode);
        mountaction.addArgument("mountpoint", mountpoint);
        mountaction.addArgument("fstype", devicefstype);
        KAuth::ActionReply mountreply = mountaction.execute();
        // qDebug() << "mount" << mountreply.errorCode() << mountreply.errorDescription();

        if (mountreply == KAuth::ActionReply::SuccessReply) {
            return int(Solid::ErrorType::NoError);
        }

        if (mountreply == KAuth::ActionReply::UserCancelled) {
            return int(Solid::ErrorType::UserCanceled);
        } else if (mountreply == KAuth::ActionReply::AuthorizationDenied) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        }
        return int(Solid::ErrorType::OperationFailed);
    }

    Solid::StorageVolume *storagevolume = device.as<Solid::StorageVolume>();
    Solid::Block *block = device.as<Solid::Block>();
    if (!storagevolume || !block) {
        return int(Solid::ErrorType::InvalidOption);
    } else if (storagevolume->fsType() == "zfs_member") {
        // create pool on USB stick, put some bad stuff on it, set mount point to / and watch it
        // unfold when mounted
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

        KAuth::Action cryptopenaction("org.kde.soliduiserver.mountunmounthelper.cryptopen");
        cryptopenaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        cryptopenaction.addArgument("device", devicenode);
        cryptopenaction.addArgument("name", deviceuuid);
        cryptopenaction.addArgument("password", passworddialog.password().toLocal8Bit().toHex());
        KAuth::ActionReply cryptopenreply = cryptopenaction.execute();

        // qDebug() << "cryptopen" << cryptopenreply.errorCode() << cryptopenreply.errorDescription();

        if (cryptopenreply == KAuth::ActionReply::UserCancelled) {
            return int(Solid::ErrorType::UserCanceled);
        } else if (cryptopenreply == KAuth::ActionReply::AuthorizationDenied) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (cryptopenreply != KAuth::ActionReply::SuccessReply) {
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
    if (!mountdir.exists(deviceuuid) && !mountdir.mkdir(deviceuuid)) {
        kWarning() << "could not create" << mountpoint;
        return int(Solid::ErrorType::OperationFailed);
    }

    KAuth::Action mountaction("org.kde.soliduiserver.mountunmounthelper.mount");
    mountaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
    mountaction.addArgument("device", devicenode);
    mountaction.addArgument("mountpoint", mountpoint);
    mountaction.addArgument("fstype", storagevolume->fsType());
    KAuth::ActionReply mountreply = mountaction.execute();
    // qDebug() << "mount" << mountreply.errorCode() << mountreply.errorDescription();

    if (mountreply == KAuth::ActionReply::SuccessReply) {
        return int(Solid::ErrorType::NoError);
    }

    if (didcryptopen) {
        KAuth::Action cryptcloseaction("org.kde.soliduiserver.mountunmounthelper.cryptclose");
        cryptcloseaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        cryptcloseaction.addArgument("name", deviceuuid);
        cryptcloseaction.execute();
    }

    if (mountreply == KAuth::ActionReply::UserCancelled) {
        return int(Solid::ErrorType::UserCanceled);
    } else if (mountreply == KAuth::ActionReply::AuthorizationDenied) {
        return int(Solid::ErrorType::UnauthorizedOperation);
    }
    return int(Solid::ErrorType::OperationFailed);
}

int SolidUiServer::unmountDevice(const QString &udi)
{
    Solid::Device device(udi);
    Solid::StorageVolume *storagevolume = device.as<Solid::StorageVolume>();
    Solid::StorageAccess *storageaccess = device.as<Solid::StorageAccess>();
    if (!storagevolume || !storageaccess) {
        return int(Solid::ErrorType::InvalidOption);
    }

    KAuth::Action unmountaction("org.kde.soliduiserver.mountunmounthelper.unmount");
    unmountaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
    unmountaction.addArgument("mountpoint", storageaccess->filePath());
    KAuth::ActionReply unmountreply = unmountaction.execute();

    // qDebug() << "unmount" << unmountreply.errorCode() << unmountreply.errorDescription();

    if (unmountreply == KAuth::ActionReply::UserCancelled) {
        return int(Solid::ErrorType::UserCanceled);
    } else if (unmountreply == KAuth::ActionReply::AuthorizationDenied) {
        return int(Solid::ErrorType::UnauthorizedOperation);
    } else if (unmountreply != KAuth::ActionReply::SuccessReply) {
        return int(Solid::ErrorType::OperationFailed);
    }

    if (storagevolume->usage() == Solid::StorageVolume::Encrypted) {
        KAuth::Action cryptcloseaction("org.kde.soliduiserver.mountunmounthelper.cryptclose");
        cryptcloseaction.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        cryptcloseaction.addArgument("name", storagevolume->uuid());
        KAuth::ActionReply cryptclosereply = cryptcloseaction.execute();

        // qDebug() << "cryptclose" << cryptclosereply.errorCode() << cryptclosereply.errorDescription();

        if (cryptclosereply == KAuth::ActionReply::UserCancelled) {
            return int(Solid::ErrorType::UserCanceled);
        } else if (cryptclosereply == KAuth::ActionReply::AuthorizationDenied) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (cryptclosereply != KAuth::ActionReply::SuccessReply) {
            return int(Solid::ErrorType::OperationFailed);
        }
    }

    return int(Solid::ErrorType::NoError);
}

#include "moc_soliduiserver.cpp"
