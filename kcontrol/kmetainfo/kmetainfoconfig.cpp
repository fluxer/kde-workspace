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

    KAboutData *about =
        new KAboutData(I18N_NOOP("KCMMetaInfo"), 0,
                       ki18n("KDE Meta Information Module"),
                       0, KLocalizedString(), KAboutData::License_GPL,
                       ki18n("Copyright 2022, Ivailo Monev <email>xakepa10@gmail.com</email>"
                       ));

    about->addAuthor(ki18n("Ivailo Monev"), KLocalizedString(), "xakepa10@gmail.com");
    setAboutData(about);

    layout()->setContentsMargins(0, 0, 0, 0);

    klistwidget->setSelectionMode(QAbstractItemView::NoSelection);
    klistwidget->setSortingEnabled(true);
    connect(klistwidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotItemChanged(QListWidgetItem*)));

    load();
}

KCMMetaInfo::~KCMMetaInfo()
{
}

void KCMMetaInfo::load()
{
    KConfig config("kmetainformationrc", KConfig::NoGlobals);
    KConfigGroup settings = config.group("Show");
    klistwidget->clear();
    foreach (const QString &key, KFileMetaInfo::supportedKeys()) {
        QListWidgetItem* item = new QListWidgetItem(KFileMetaInfo::name(key), klistwidget);
        item->setData(Qt::UserRole, key);
        const bool show = settings.readEntry(key, true);
        item->setCheckState(show ? Qt::Checked : Qt::Unchecked);
    }

    emit changed(false);
}

void KCMMetaInfo::save()
{
    KConfig config("kmetainformationrc", KConfig::NoGlobals);
    KConfigGroup settings = config.group("Show");
    for (int i = 0; i < klistwidget->count(); ++i) {
        QListWidgetItem* item = klistwidget->item(i);
        const bool show = (item->checkState() == Qt::Checked);
        const QString key = item->data(Qt::UserRole).toString();
        settings.writeEntry(key, show);
    }
    settings.sync();

    emit changed(false);
}

void KCMMetaInfo::slotItemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item);
    emit changed(true);
}

#include "moc_kmetainfoconfig.cpp"
