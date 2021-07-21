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
#include <solid/solidnamespace.h>

#include "deviceactionsdialog.h"
#include "deviceaction.h"
#include "deviceserviceaction.h"
#include "devicenothingaction.h"

#include <QProcess>
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
    Solid::StorageVolume *storagevolume = device.as<Solid::StorageVolume>();
    Solid::Block *block = device.as<Solid::Block>();
    if (!storagevolume || !block) {
        return int(Solid::ErrorType::InvalidOption);
    }

    QString devicenode = block->device();
    if (storagevolume->usage() == Solid::StorageVolume::Encrypted) {
        KPasswordDialog passworddialog(0, KPasswordDialog::NoFlags);

        QString label = device.vendor();
        if (!label.isEmpty()) {
            label += ' ';
        }
        label += device.product();

        passworddialog.setPrompt(i18n("'%1' needs a password to be accessed. Please enter a password.", label));
        passworddialog.setPixmap(KIcon(device.icon()).pixmap(64, 64));
        if (!passworddialog.exec()) {
            return int(Solid::ErrorType::UserCanceled);
        }

        KAuth::Action action("org.kde.soliduiserver.mountunmounthelper.cryptopen");
        action.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        action.addArgument("device", block->device());
        action.addArgument("name", storagevolume->uuid());
        action.addArgument("password", passworddialog.password().toLocal8Bit().toHex());
        KAuth::ActionReply reply = action.execute();

        // qDebug() << "cryptopen" << reply.errorCode() << reply.errorDescription() << reply.type();

        if (reply == KAuth::ActionReply::UserCancelled) {
            return int(Solid::ErrorType::UserCanceled);
        } else if (reply == KAuth::ActionReply::AuthorizationDenied) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (reply != KAuth::ActionReply::SuccessReply) {
            return int(Solid::ErrorType::OperationFailed);
        }

        // NOTE: keep in sync with kdelibs/solid/solid/backends/udev/udevstorageaccess.cpp
        devicenode = QLatin1String("/dev/mapper/") + storagevolume->uuid();
    }

    // permission denied on /run/mount so.. using base directory that is writable
    const QString mountbase = KGlobal::dirs()->saveLocation("tmp");
    const QString deviceuuid(storagevolume->uuid());
    QString mountpoint = mountbase + QLatin1Char('/') + deviceuuid;
    QDir mountdir(mountbase);
    if (!mountdir.exists(deviceuuid) && !mountdir.mkdir(deviceuuid)) {
        kWarning() << "could not create" << mountpoint;
        return int(Solid::ErrorType::OperationFailed);
    }

    KAuth::Action action("org.kde.soliduiserver.mountunmounthelper.mount");
    action.setHelperID("org.kde.soliduiserver.mountunmounthelper");
    action.addArgument("device", devicenode);
    action.addArgument("mountpoint", mountpoint);
    KAuth::ActionReply reply = action.execute();

    // qDebug() << "mount" << reply.errorCode() << reply.errorDescription() << reply.type();

    if (reply == KAuth::ActionReply::SuccessReply) {
        return int(Solid::ErrorType::NoError);
    } else if (reply == KAuth::ActionReply::UserCancelled) {
        return int(Solid::ErrorType::UserCanceled);
    } else if (reply == KAuth::ActionReply::AuthorizationDenied) {
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

    KAuth::Action action("org.kde.soliduiserver.mountunmounthelper.unmount");
    action.setHelperID("org.kde.soliduiserver.mountunmounthelper");
    action.addArgument("mountpoint", storageaccess->filePath());
    KAuth::ActionReply reply = action.execute();

    // qDebug() << "unmount" << reply.errorCode() << reply.errorDescription();

    if (reply == KAuth::ActionReply::UserCancelled) {
        return int(Solid::ErrorType::UserCanceled);
    } else if (reply == KAuth::ActionReply::AuthorizationDenied) {
        return int(Solid::ErrorType::UnauthorizedOperation);
    } else if (reply != KAuth::ActionReply::SuccessReply) {
        return int(Solid::ErrorType::OperationFailed);
    }

    if (storagevolume->usage() == Solid::StorageVolume::Encrypted) {
        KAuth::Action action("org.kde.soliduiserver.mountunmounthelper.cryptclose");
        action.setHelperID("org.kde.soliduiserver.mountunmounthelper");
        action.addArgument("name", storagevolume->uuid());
        KAuth::ActionReply reply = action.execute();

        // qDebug() << "cryptclose" << reply.errorCode() << reply.errorDescription() << reply.type();

        if (reply == KAuth::ActionReply::UserCancelled) {
            return int(Solid::ErrorType::UserCanceled);
        } else if (reply == KAuth::ActionReply::AuthorizationDenied) {
            return int(Solid::ErrorType::UnauthorizedOperation);
        } else if (reply != KAuth::ActionReply::SuccessReply) {
            return int(Solid::ErrorType::OperationFailed);
        }
    }

    return int(Solid::ErrorType::NoError);
}

#include "moc_soliduiserver.cpp"
