/*
 * nic.cpp
 *
 *  Copyright (C) 2001 Alexander Neundorf <neundorf@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "nic.h"

#include <kaboutdata.h>
#include <kdialog.h>
#include <kglobal.h>

#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QNetworkInterface>

#include <KPluginFactory>
#include <KPluginLoader>


K_PLUGIN_FACTORY(KCMNicFactory, registerPlugin<KCMNic>();)
K_EXPORT_PLUGIN(KCMNicFactory("kcmnic"))

struct MyNIC {
    QString name;
    QString addr;
    QString netmask;
    QString state;
    QString type;
    QString HWaddr;
};

QList<MyNIC*> findNICs();

KCMNic::KCMNic(QWidget *parent, const QVariantList &)
    : KCModule(KCMNicFactory::componentData(), parent) {
    QVBoxLayout *box=new QVBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(KDialog::spacingHint());
    m_list=new QTreeWidget(this);
    m_list->setRootIsDecorated(false);
    box->addWidget(m_list);
    QStringList columns;
    columns << i18n("Name") << i18n("IP Address") << i18n("Network Mask") << i18n("Type") << i18n("State") << i18n("HWAddr");
    m_list->setHeaderLabels(columns);
    QHBoxLayout *hbox = new QHBoxLayout();
    box->addItem(hbox);
    m_updateButton = new QPushButton(i18n("&Update"),this);
    hbox->addWidget(m_updateButton);
    hbox->addStretch(1);
    QTimer* timer = new QTimer(this);
    timer->start(60000);
    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    update();
    KAboutData *about = new KAboutData(I18N_NOOP("kcminfo"), 0,
        ki18n("System Information Control Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("(c) 2001 - 2002 Alexander Neundorf"));

    about->addAuthor(ki18n("Alexander Neundorf"), KLocalizedString(), "neundorf@kde.org");
    setAboutData(about);
}

void KCMNic::update()
{
    m_list->clear();
    QList<MyNIC*> nics = findNICs();

    foreach(MyNIC* tmp, nics) {
        QStringList lst;
        lst << tmp->name << tmp->addr << tmp->netmask << tmp->type << tmp->state << tmp->HWaddr;
        new QTreeWidgetItem(m_list,lst);

        delete tmp;
    }

    nics.clear();
}

QList<MyNIC*> findNICs()
{
    QString upMessage(i18nc("State of network card is connected", "Up") );
    QString downMessage(i18nc("State of network card is disconnected", "Down") );

    QList<MyNIC*> nl;

    foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces()) {
        const QNetworkInterface::InterfaceFlags flags = iface.flags();
        const QList<QNetworkAddressEntry> adresses = iface.addressEntries();

        MyNIC *tmp = new MyNIC;
        tmp->name = iface.name();
        tmp->addr = (!adresses.isEmpty() ? adresses.first().ip().toString() : QString());
        tmp->netmask = (!adresses.isEmpty() ? adresses.first().netmask().toString() : QString());
        if (tmp->netmask.isEmpty()) {
            tmp->netmask = i18nc("Unknown network mask", "Unknown");
        }
        tmp->state = (flags & QNetworkInterface::IsUp) ? upMessage : downMessage;

        if (flags & QNetworkInterface::CanBroadcast)
            tmp->type=i18nc("@item:intext Mode of network card", "Broadcast");
        else if (flags & QNetworkInterface::IsPointToPoint)
            tmp->type=i18nc("@item:intext Mode of network card", "Point to Point");
        else if (flags & QNetworkInterface::CanMulticast)
            tmp->type=i18nc("@item:intext Mode of network card", "Multicast");
        else if (flags & QNetworkInterface::IsLoopBack)
            tmp->type=i18nc("@item:intext Mode of network card", "Loopback");
        else
            tmp->type=i18nc("@item:intext Mode of network card", "Unknown");

        tmp->HWaddr = iface.hardwareAddress();
        if (tmp->HWaddr.isEmpty()) {
            tmp->HWaddr = i18nc("Unknown HWaddr", "Unknown");
        }

        nl.append(tmp);
    }

    return nl;
}

#include "moc_nic.cpp"
