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

#include "kmetainfoconfig.h"

#include <kdebug.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kfilemetainfo.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kservicetypetrader.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY(KCMMetaInfoFactory, registerPlugin<KCMMetaInfo>();)
K_EXPORT_PLUGIN(KCMMetaInfoFactory("kcmmetainfoconfig", "kcm_metainfoconfig"))

KCMMetaInfo::KCMMetaInfo(QWidget* parent, const QVariantList& args)
    : KCModule(KCMMetaInfoFactory::componentData(), parent)
{
    Q_UNUSED(args);

    setQuickHelp( i18n("<h1>KFileMetaInfo</h1>"
            "This module allows you to change KDE meta information preferences."));

    setupUi(this);

    KAboutData *about = new KAboutData(
        I18N_NOOP("KCMMetaInfo"), 0,
        ki18n("KDE Meta Information Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );

    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    layout()->setContentsMargins(0, 0, 0, 0);

    pluginslist->setSelectionMode(QAbstractItemView::NoSelection);
    pluginslist->setSortingEnabled(true);
    connect(pluginslist, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotItemChanged(QListWidgetItem*)));

    metainfolist->setSelectionMode(QAbstractItemView::NoSelection);
    metainfolist->setSortingEnabled(true);
    connect(metainfolist, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotItemChanged(QListWidgetItem*)));

    load();
}

KCMMetaInfo::~KCMMetaInfo()
{
}

void KCMMetaInfo::load()
{
    {
        KConfig config("kmetainformationrc", KConfig::NoGlobals);
        pluginslist->clear();
        KConfigGroup pluginsgroup = config.group("Plugins");
        const KService::List kfmdplugins = KServiceTypeTrader::self()->query("KFileMetaData/Plugin");
        foreach (const KService::Ptr &kfmdplugin, kfmdplugins) {
            const QString key = kfmdplugin->desktopEntryName();
            const QString itemtext = kfmdplugin->genericName();
            QListWidgetItem* item = new QListWidgetItem(itemtext, pluginslist);
            item->setData(Qt::UserRole, key);
            const bool enable = pluginsgroup.readEntry(key, true);
            item->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
        }
    }

    loadMetaInfo();

    emit changed(false);
}

void KCMMetaInfo::save()
{
    {
        KConfig config("kmetainformationrc", KConfig::NoGlobals);
        KConfigGroup pluginsgroup = config.group("Plugins");
        for (int i = 0; i < pluginslist->count(); ++i) {
            QListWidgetItem* item = pluginslist->item(i);
            const bool enable = (item->checkState() == Qt::Checked);
            const QString key = item->data(Qt::UserRole).toString();
            pluginsgroup.writeEntry(key, enable);
        }

        KConfigGroup showgroup = config.group("Show");
        for (int i = 0; i < metainfolist->count(); ++i) {
            QListWidgetItem* item = metainfolist->item(i);
            const bool show = (item->checkState() == Qt::Checked);
            const QString key = item->data(Qt::UserRole).toString();
            showgroup.writeEntry(key, show);
        }
    }

    emit changed(false);

    loadMetaInfo();
}

void KCMMetaInfo::slotItemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item);
    emit changed(true);
}

void KCMMetaInfo::loadMetaInfo()
{
    metainfolist->clear();
    KConfig config("kmetainformationrc", KConfig::NoGlobals);
    KConfigGroup showgroup = config.group("Show");
    foreach (const QString &key, KFileMetaInfo::supportedKeys()) {
        QListWidgetItem* item = new QListWidgetItem(KFileMetaInfo::name(key), metainfolist);
        item->setData(Qt::UserRole, key);
        const bool show = showgroup.readEntry(key, true);
        item->setCheckState(show ? Qt::Checked : Qt::Unchecked);
    }
}

#include "moc_kmetainfoconfig.cpp"
