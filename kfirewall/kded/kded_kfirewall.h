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

#ifndef KFIREWALL_KDED_H
#define KFIREWALL_KDED_H

#include <QFileSystemWatcher>
#include <kdedmodule.h>

class KFirewallModule: public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kfirewall")

public:
    KFirewallModule(QObject *parent, const QList<QVariant>&);
    ~KFirewallModule();

public Q_SLOTS:
    Q_SCRIPTABLE bool enable();
    Q_SCRIPTABLE bool disable();

private Q_SLOTS:
    void slotFileChanged(const QString &path);

private:
    QString m_kfirewallconfigpath;
    QVariantMap m_kfirewallsettingsmap;
    QFileSystemWatcher m_watcher;
};

#endif // KFIREWALL_KDED_H
