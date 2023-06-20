/*  This file is part of the KDE project
    Copyright (C) 2022 Ivailo Monev <xakepa10@gmail.com>

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

#include "kded_kdirshare.h"
#include "kdirshare.h"

#include <QTimer>
#include <klocale.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <kdirnotify.h>
#include <kpluginfactory.h>
#include <kdebug.h>

static QByteArray getDirShareKey(const QString &kdirsharedirpath)
{
    return kdirsharedirpath.toLocal8Bit().toHex();
}

K_PLUGIN_FACTORY(KDirShareModuleFactory, registerPlugin<KDirShareModule>();)
K_EXPORT_PLUGIN(KDirShareModuleFactory("kdirshare"))

KDirShareModule::KDirShareModule(QObject *parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    m_passwdstore.setStoreID("KDirShare");
    // delayed restore without blocking
    QTimer::singleShot(2000, this, SLOT(slotDelayedRestore()));
}

KDirShareModule::~KDirShareModule()
{
    qDeleteAll(m_dirshares);
    m_dirshares.clear();
}

QString KDirShareModule::share(const QString &dirpath,
                               const uint portmin, const uint portmax,
                               const QString &username, const QString &password)
{
    if (isShared(dirpath)) {
        const QString unshareerror = unshare(dirpath);
        if (!unshareerror.isEmpty()) {
            return unshareerror;
        }
    }

    KDirShareImpl *kdirshareimpl = new KDirShareImpl(this);
    // qDebug() << Q_FUNC_INFO << dirpath << portmin << portmax;
    const QString serveerror = kdirshareimpl->serve(
        dirpath,
        portmin, portmax,
        username, password
    );
    if (!username.isEmpty() && !password.isEmpty()) {
        m_passwdstore.storePasswd(KPasswdStore::makeKey(dirpath), password);
    }
    if (!serveerror.isEmpty()) {
        delete kdirshareimpl;
        return serveerror;
    }
    m_dirshares.append(kdirshareimpl);
    org::kde::KDirNotify::emitFilesAdded(QString::fromLatin1("network:/"));
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    const QByteArray kdirsharekey = getDirShareKey(kdirshareimpl->directory());
    KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
    // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirshareimpl->directory() << kdirshareimpl->portMin() << kdirshareimpl->portMax();
    kdirsharegroup.writeEntry("dirpath", kdirshareimpl->directory());
    kdirsharegroup.writeEntry("portmin", uint(kdirshareimpl->portMin()));
    kdirsharegroup.writeEntry("portmax", uint(kdirshareimpl->portMax()));
    kdirsharegroup.writeEntry("user", kdirshareimpl->user());
    return QString();
}

QString KDirShareModule::unshare(const QString &dirpath)
{
    foreach (KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
            const QByteArray kdirsharekey = getDirShareKey(kdirshareimpl->directory());
            KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
            kdirsharegroup.writeEntry("dirpath", QString());
            delete kdirshareimpl;
            m_dirshares.removeAll(kdirshareimpl);
            org::kde::KDirNotify::emitFilesAdded(QString::fromLatin1("network:/")); // works the same as emitFilesRemoved()
            return QString();
        }
    }
    return i18n("Invalid directory share: %1", dirpath);
}

bool KDirShareModule::isShared(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return true;
        }
    }
    return false;
}

quint16 KDirShareModule::getPortMin(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return kdirshareimpl->portMin();
        }
    }
    return s_kdirshareportmin;
}

quint16 KDirShareModule::getPortMax(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return kdirshareimpl->portMax();
        }
    }
    return s_kdirshareportmax;
}

QString KDirShareModule::getUser(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return kdirshareimpl->user();
        }
    }
    return QString();
}

QString KDirShareModule::getPassword(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return kdirshareimpl->password();
        }
    }
    return QString();
}

QString KDirShareModule::getAddress(const QString &dirpath) const
{
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            return kdirshareimpl->address();
        }
    }
    return QString();
}

void KDirShareModule::slotDelayedRestore()
{
    bool requiresauth = false;
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    const QStringList kdirshareconfiggroups = kdirshareconfig.groupList();
    foreach (const QString &kdirsharekey, kdirshareconfiggroups) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        if (kdirsharedirpath.isEmpty()) {
            continue;
        }
        const QString kdirshareuser = kdirsharegroup.readEntry("user", QString());
        if (!kdirshareuser.isEmpty()) {
            requiresauth = true;
            break;
        }
    }
    if (requiresauth) {
        if (!m_passwdstore.openStore()) {
            KNotification *knotification = new KNotification("AuthError");
            knotification->setComponentData(KComponentData("kdirshare"));
            knotification->setTitle(i18n("Directory share"));
            knotification->setText(i18n("Authorization is required but could not open password store"));
            knotification->sendEvent();
            return;
        }
    }

    bool shareerror = false;
    foreach (const QString &kdirsharekey, kdirshareconfiggroups) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        if (kdirsharedirpath.isEmpty()) {
            continue;
        }
        const uint kdirshareportmin = kdirsharegroup.readEntry("portmin", uint(s_kdirshareportmin));
        const uint kdirshareportmax = kdirsharegroup.readEntry("portmax", uint(s_kdirshareportmax));
        const QString kdirshareuser = kdirsharegroup.readEntry("user", QString());
        QString kdirsharepassword;
        if (!kdirshareuser.isEmpty()) {
            kdirsharepassword = m_passwdstore.getPasswd(KPasswdStore::makeKey(kdirsharedirpath));
        }
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharedirpath << kdirshareportmin << kdirshareportmax;
        const QString kdirshareerror = share(
            kdirsharedirpath,
            quint16(kdirshareportmin), quint16(kdirshareportmax),
            kdirshareuser, kdirsharepassword
        );
        if (!kdirshareerror.isEmpty()) {
            kWarning() << kdirshareerror;
            shareerror = true;
        }
    }
    if (shareerror) {
        KNotification *knotification = new KNotification("ShareError");
        knotification->setComponentData(KComponentData("kdirshare"));
        knotification->setTitle(i18n("Directory share"));
        knotification->setText(i18n("Unable to share one or more directories"));
        knotification->sendEvent();
    }
}
