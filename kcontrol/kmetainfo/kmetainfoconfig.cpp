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

    setQuickHelp(i18n("<h1>KFileMetaInfo</h1> This module allows you to change KDE meta information preferences."));

    setupUi(this);

    KAboutData *about = new KAboutData(
        I18N_NOOP("KCMMetaInfo"), 0,
        ki18n("KDE Meta Information Module"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>")
    );
    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    setButtons(KCModule::Help | KCModule::Apply);

    connect(pluginstable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotPluginItemChanged(QTableWidgetItem*)));

    connect(metainfolist, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotMetaItemChanged(QListWidgetItem*)));

    load();
}

KCMMetaInfo::~KCMMetaInfo()
{
}

void KCMMetaInfo::load()
{
    {
        KConfig config("kmetainformationrc", KConfig::NoGlobals);
        pluginstable->clearContents();
        pluginstable->setRowCount(0);
        int rowcount = 0;
        KConfigGroup pluginsgroup = config.group("Plugins");
        const KService::List kfmdplugins = KServiceTypeTrader::self()->query("KFileMetaData/Plugin");
        foreach (const KService::Ptr &kfmdplugin, kfmdplugins) {
            const QString key = kfmdplugin->desktopEntryName();

            pluginstable->setRowCount(rowcount + 1);

            QTableWidgetItem* nameitem = new QTableWidgetItem();
            nameitem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            nameitem->setText(kfmdplugin->genericName());
            nameitem->setData(Qt::UserRole, key);
            const bool enable = pluginsgroup.readEntry(key, true);
            nameitem->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
            pluginstable->setItem(rowcount, 0, nameitem);

            QTableWidgetItem* descitem = new QTableWidgetItem();
            descitem->setFlags(Qt::ItemIsEnabled);
            descitem->setText(kfmdplugin->comment());
            pluginstable->setItem(rowcount, 1, descitem);

            rowcount++;
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
        for (int i = 0; i < pluginstable->rowCount(); ++i) {
            QTableWidgetItem* item = pluginstable->item(i, 0);
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

void KCMMetaInfo::slotPluginItemChanged(QTableWidgetItem *item)
{
    Q_UNUSED(item);
    emit changed(true);
}

void KCMMetaInfo::slotMetaItemChanged(QListWidgetItem *item)
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
