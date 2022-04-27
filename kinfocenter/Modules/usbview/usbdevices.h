/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __USB_DEVICES_H__
#define __USB_DEVICES_H__

#include <QList>
#include <QString>

class USBDB;

class USBDevice
{
public:
    USBDevice();
    ~USBDevice();

    int level() const {
        return _level;
    }
    int device() const {
        return _device;
    }
    int parent() const {
        return _parent;
    }
    int bus() const {
        return _bus;
    }
    QString product();

    QString dump();

    static QList<USBDevice*> &devices() {
        return _devices;
    }
    static USBDevice *find(int bus, int device);
    static bool init();

private:
    static QList<USBDevice*> _devices;

    static USBDB *_db;

    int _bus, _level, _parent, _port, _device, _channels;
    float _speed;

    QString _serial;

    unsigned int _class, _sub, _prot, _maxPacketSize;

    unsigned int _vendorID, _prodID;

    QString _ver, _rev;
};

#endif
