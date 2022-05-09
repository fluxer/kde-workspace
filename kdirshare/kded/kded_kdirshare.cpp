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

#include <klocale.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <kpluginfactory.h>
#include <kdebug.h>

K_PLUGIN_FACTORY(KDirShareModuleFactory, registerPlugin<KDirShareModule>();)
K_EXPORT_PLUGIN(KDirShareModuleFactory("kdirshare"))

KDirShareModule::KDirShareModule(QObject *parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    bool shareerror = false;
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    for (const QString &kdirsharekey: kdirshareconfig.groupList()) {
        // qDebug() << Q_FUNC_INFO << kdirsharekey;
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        const QString kdirsharedirpath = kdirsharegroup.readEntry("dirpath", QString());
        const uint kdirshareportmin = kdirsharegroup.readEntry("portmin", uint(s_kdirshareportmin));
        const uint kdirshareportmax = kdirsharegroup.readEntry("portmax", uint(s_kdirshareportmax));
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirsharedirpath << kdirshareportmin << kdirshareportmax;
        const QString kdirshareerror = share(
            kdirsharedirpath,
            quint16(kdirshareportmin), quint16(kdirshareportmax)
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

KDirShareModule::~KDirShareModule()
{
    KConfig kdirshareconfig("kdirsharerc", KConfig::SimpleConfig);
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        const QByteArray kdirsharekey = kdirshareimpl->directory().toLocal8Bit().toHex();
        KConfigGroup kdirsharegroup = kdirshareconfig.group(kdirsharekey);
        // qDebug() << Q_FUNC_INFO << kdirsharekey << kdirshareimpl->directory() << kdirshareimpl->portMin() << kdirshareimpl->portMax();
        kdirsharegroup.writeEntry("dirpath", kdirshareimpl->directory());
        kdirsharegroup.writeEntry("portmin", kdirshareimpl->portMin());
        kdirsharegroup.writeEntry("portmax", kdirshareimpl->portMax());
    }
    qDeleteAll(m_dirshares);
    m_dirshares.clear();
}

QString KDirShareModule::share(const QString &dirpath, const uint portmin, const uint portmax)
{
    if (isShared(dirpath)) {
        const QString unshareerror = unshare(dirpath);
        if (!unshareerror.isEmpty()) {
            return unshareerror;
        }
    }

    KDirShareImpl *kdirshareimpl = new KDirShareImpl(this);
    if (!kdirshareimpl->setDirectory(dirpath)) {
        delete kdirshareimpl;
        return i18n("Directory does not exist: %1", dirpath);
    }
    // qDebug() << Q_FUNC_INFO << serverport;
    if (!kdirshareimpl->serve(QHostAddress(QHostAddress::Any), portmin, portmax)) {
        delete kdirshareimpl;
        return i18n("Could not serve: %1", kdirshareimpl->errorString());
    }
    if (!kdirshareimpl->publish()) {
        kdirshareimpl->stop();
        delete kdirshareimpl;
        return i18n("Could not publish service for: %1", dirpath);
    }
    m_dirshares.append(kdirshareimpl);
    return QString();
}

QString KDirShareModule::unshare(const QString &dirpath)
{
    foreach (KDirShareImpl *kdirshareimpl, m_dirshares) {
        if (kdirshareimpl->directory() == dirpath) {
            kdirshareimpl->stop();
            delete kdirshareimpl;
            m_dirshares.removeAll(kdirshareimpl);
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

#include "moc_kded_kdirshare.cpp"
