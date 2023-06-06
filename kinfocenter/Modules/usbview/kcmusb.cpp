/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGroupBox>
#include <QLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>
#include <QHBoxLayout>
#include <QList>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDebug>

#include <kaboutdata.h>
#include <kdialog.h>
#include <KPluginFactory>
#include <KPluginLoader>

#include "usbdevices.h"
#include "moc_kcmusb.cpp"

class KScopedStop
{
public:
    KScopedStop(QTimer *timer);
    ~KScopedStop();
private:
    QTimer *m_timer;
};

KScopedStop::KScopedStop(QTimer *timer)
    : m_timer(timer)
{
    m_timer->stop();
}

KScopedStop::~KScopedStop()
{
    // 1 sec seems to be a good compromise between latency and polling load.
    m_timer->start(1000);
}

K_PLUGIN_FACTORY(USBFactory, registerPlugin<USBViewer>();)
K_EXPORT_PLUGIN(USBFactory("kcmusb"))

USBViewer::USBViewer(QWidget *parent, const QVariantList &args)
    : KCModule(USBFactory::componentData(), parent),
    _lastdevicecount(0),
    _refreshTimer(nullptr)
{
    Q_UNUSED(args);

    setQuickHelp(i18n("This module allows you to see the devices attached to your USB bus(es)."));
    setButtons(KCModule::Help | KCModule::Export);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QSplitter *splitter = new QSplitter(this);
    splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    mainLayout->addWidget(splitter);

    _refreshTimer = new QTimer(this);
    _devices = new QTreeWidget(splitter);
    
    QStringList headers;
    headers << i18n("Device");
    _devices->setHeaderLabels(headers);
    _devices->setRootIsDecorated(true);
    _devices->header()->hide();
    //_devices->setColumnWidthMode(0, Q3ListView::Maximum);

    QList<int> sizes;
    sizes.append(200);
    splitter->setSizes(sizes);

    _details = new QTextEdit(splitter);
    _details->setReadOnly(true);

    connect(_refreshTimer, SIGNAL(timeout()), SLOT(refresh()));
    connect(_devices, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(selectionChanged(QTreeWidgetItem*)));

    KAboutData *about = new KAboutData(I18N_NOOP("kcmusb"), 0, ki18n("KDE USB Viewer"),
                    0, KLocalizedString(), KAboutData::License_GPL,
                    ki18n("(c) 2001 Matthias Hoelzer-Kluepfel"));

    about->addAuthor(ki18n("Matthias Hoelzer-Kluepfel"), KLocalizedString(), "mhk@kde.org");
    about->addCredit(ki18n("Leo Savernik"), ki18n("Live Monitoring of USB Bus"), "l.savernik@aon.at");
    setAboutData(about);
}

void USBViewer::load()
{
    refresh();
}

static quint32 key(USBDevice &dev)
{
    return dev.bus()*256 + dev.device();
}

static quint32 key_parent(USBDevice &dev)
{
    return dev.bus()*256 + dev.parent();
}

static void delete_recursive(QTreeWidgetItem *item, const QMap<int, QTreeWidgetItem*> &new_items)
{
    if (!item) {
        return;
    }

    QTreeWidgetItemIterator it(item, QTreeWidgetItemIterator::All);
    while ( *it != NULL ) {
        QTreeWidgetItem* currentItem = *it;
        if (new_items.contains(currentItem->text(1).toUInt()) == false) {
            delete_recursive(currentItem->child(0), new_items);
            delete currentItem;
        }
        ++it;
    }
}

void USBViewer::refresh()
{
    KScopedStop kscopedstop(_refreshTimer);

    USBDevice::init();
    const int currentdevicecount = USBDevice::devices().size();
    if (currentdevicecount == _lastdevicecount) {
        return;
    }
    _lastdevicecount = currentdevicecount;

    _items.clear();
    _devices->clear();

    QMap<int, QTreeWidgetItem*> new_items;

    int level = 0;
    bool found = true;

    while (found) {
        found = false;

        foreach(USBDevice* usbDevice, USBDevice::devices()) {
            if (usbDevice->level() == level) {
                quint32 k = key(*usbDevice);
                if (level == 0) {
                    QTreeWidgetItem* item = _items.value(k);
                    if (!item) {
                        QStringList itemContent;
                        itemContent << usbDevice->product() << QString::number(k);
                        item = new QTreeWidgetItem(_devices, itemContent);
                    }
                    new_items.insert(k, item);
                    found = true;
                } else {
                    QTreeWidgetItem *parent = new_items.value(key_parent(*usbDevice));
                    if (parent) {
                        QTreeWidgetItem *item = _items.value(k);

                        if (!item) {
                            QStringList itemContent;
                            itemContent << usbDevice->product() << QString::number(k);
                            item = new QTreeWidgetItem(parent, itemContent);
                        }
                        new_items.insert(k, item);
                        parent->setExpanded(true);
                        found = true;
                    }
                }
            }
        }

        ++level;
    }

    // recursive delete all items not in new_items
    delete_recursive(_devices->topLevelItem(0), new_items);

    _items = new_items;

    if (_devices->selectedItems().isEmpty() == true) {
        selectionChanged(_devices->topLevelItem(0));
    }

    // TODO: export all devices information
    setExportText(_details->toPlainText());
    // qDebug() << Q_FUNC_INFO << _details->toPlainText();
}

void USBViewer::selectionChanged(QTreeWidgetItem *item)
{
    if (item) {
        quint32 busdev = item->text(1).toUInt();
        USBDevice *dev = USBDevice::find(busdev>>8, busdev&255);
        if (dev) {
            _details->setHtml(dev->dump());
            return;
        }
    }
    _details->clear();
}
