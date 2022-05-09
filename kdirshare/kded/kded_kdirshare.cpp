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
#include <ksettings.h>
#include <kpluginfactory.h>
#include <kdebug.h>

static quint16 getRandomPort()
{
    quint16 portnumber = 0;
    while (portnumber < 1000 || portnumber > 30000) {
        portnumber = quint16(qrand());
    }
    return portnumber;
}

K_PLUGIN_FACTORY(KDirShareModuleFactory, registerPlugin<KDirShareModule>();)
K_EXPORT_PLUGIN(KDirShareModuleFactory("kdirshare"))

KDirShareModule::KDirShareModule(QObject *parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    KSettings kdirsharesettings("kdirsharerc", KSettings::SimpleConfig);
    foreach (const QString &kdirsharekey, kdirsharesettings.keys()) {
        const QString kdirsharedir = kdirsharesettings.value(kdirsharekey).toString();
        const QString kdirshareerror = share(kdirsharedir);
        if (!kdirshareerror.isEmpty()) {
            kWarning() << kdirshareerror;
        }
    }
}

KDirShareModule::~KDirShareModule()
{
    KSettings kdirsharesettings("kdirsharerc", KSettings::SimpleConfig);
    foreach (const KDirShareImpl *kdirshareimpl, m_dirshares) {
        const QByteArray kdirsharekey = kdirshareimpl->directory().toLocal8Bit().toHex();
        kdirsharesettings.setValue(kdirsharekey, kdirshareimpl->directory());
    }
    qDeleteAll(m_dirshares);
}

QString KDirShareModule::share(const QString &dirpath)
{
    KDirShareImpl *kdirshareimpl = new KDirShareImpl(this);
    if (!kdirshareimpl->setDirectory(dirpath)) {
        kdirshareimpl->deleteLater();
        return i18n("Directory does not exist: %1", dirpath);
    }
    const quint16 randomport = getRandomPort();
    // qDebug() << Q_FUNC_INFO << randomport;
    if (!kdirshareimpl->serve(QHostAddress(QHostAddress::Any), randomport)) {
        kdirshareimpl->deleteLater();
        return i18n("Could not serve: %1", kdirshareimpl->errorString());
    }
    if (!kdirshareimpl->publish()) {
        kdirshareimpl->stop();
        kdirshareimpl->deleteLater();
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
            kdirshareimpl->deleteLater();
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

#include "moc_kded_kdirshare.cpp"
