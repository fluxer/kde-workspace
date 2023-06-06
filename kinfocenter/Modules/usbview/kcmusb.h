/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KCMUSB_H
#define KCMUSB_H

#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextEdit>
#include <QTimer>

#include <kcmodule.h>

class USBViewer : public KCModule
{
    Q_OBJECT
public:
    explicit USBViewer(QWidget *parent = 0L, const QVariantList &list=QVariantList());

    void load();

protected Q_SLOTS:
    void selectionChanged(QTreeWidgetItem *item);
    void refresh();

private:
    int _lastdevicecount;
    QMap<int, QTreeWidgetItem*> _items;
    QTreeWidget *_devices;
    QTextEdit *_details;
    QTimer *_refreshTimer;
};

#endif
