/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usbdevices.h"

#include <sys/types.h>

#include <klocale.h>
#include <kdebug.h>

#include "usbdb.h"

#include <libusb.h>

QList<USBDevice*> USBDevice::_devices;
USBDB *USBDevice::_db;

static float getSpeed(int number)
{
    switch (number) {
        case LIBUSB_SPEED_LOW: {
            return 1.5;
        }
        case LIBUSB_SPEED_FULL: {
            return 12.0;
        }
        case LIBUSB_SPEED_HIGH: {
            return 480.0;
        }
        case LIBUSB_SPEED_SUPER: {
            return 5000.0;
        }
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_DRAGONFLY)
        case LIBUSB_SPEED_SUPER_PLUS: {
            return 10000.0;
        }
#endif
        case LIBUSB_SPEED_UNKNOWN: {
            return 0.0;
        }
    }
    kWarning() << "Unknown libusb speed" << number;
    return 0.0;
}

static QString getVersion(uint16_t number)
{
    uint8_t *numberarray = reinterpret_cast<uint8_t *>(&number);
    return QString::fromLatin1("%1.%2").arg(QString::number(numberarray[1], 16)).arg(QString::number(numberarray[0], 16));
}

USBDevice::USBDevice() :
    _bus(0), _level(0), _parent(0), _port(0), _device(0), _channels(0),
    _speed(0.0), _class(0), _sub(0), _prot(0), _maxPacketSize(0),
    _vendorID(0), _prodID(0)
{
    _devices.append(this);

    if (!_db) {
        _db = new USBDB;
    }
}

USBDevice::~USBDevice() {
}

USBDevice* USBDevice::find(int bus, int device) {
    foreach(USBDevice* usbDevice, _devices) {
        if (usbDevice->bus() == bus && usbDevice->device() == device) {
            return usbDevice;
        }
    }

    return NULL;
}

QString USBDevice::product() {
    QString pname = _db->device(_vendorID, _prodID);
    if (!pname.isEmpty()) {
        return pname;
    }
    return i18n("Unknown");
}

QString USBDevice::dump() {
    QString r;

    r = "<qml><h2><center>" + product() + "</center></h2><br/><hl/>";

    if (!_serial.isEmpty()) {
        r += i18n("<b>Serial #:</b> ") + _serial + "<br/>";
    }

    r += "<br/><table>";

    QString c = QString("<td>%1</td>").arg(_class);
    QString cname = _db->cls(_class);
    if (!cname.isEmpty()) {
        c += "<td>(" + i18n(cname.toLatin1()) +")</td>";
    }
    r += i18n("<tr><td><i>Class</i></td>%1</tr>", c);
    QString sc = QString("<td>%1</td>").arg(_sub);
    QString scname = _db->subclass(_class, _sub);
    if (!scname.isEmpty()) {
        sc += "<td>(" + i18n(scname.toLatin1()) +")</td>";
    }
    r += i18n("<tr><td><i>Subclass</i></td>%1</tr>", sc);
    QString pr = QString("<td>%1</td>").arg(_prot);
    QString prname = _db->protocol(_class, _sub, _prot);
    if (!prname.isEmpty()) {
        pr += "<td>(" + prname +")</td>";
    }
    r += i18n("<tr><td><i>Protocol</i></td>%1</tr>", pr);
    r += i18n("<tr><td><i>USB Version</i></td><td>%1</td></tr>", _ver);
    r += "<tr><td></td></tr>";

    QString v = QString::number(_vendorID, 16);
    QString name = _db->vendor(_vendorID);
    if (!name.isEmpty()) {
        v += "<td>(" + name +")</td>";
    }
    r += i18n("<tr><td><i>Vendor ID</i></td><td>0x%1</td></tr>", v);
    QString p = QString::number(_prodID, 16);
    QString pname = _db->device(_vendorID, _prodID);
    if (!pname.isEmpty()) {
        p += "<td>(" + pname +")</td>";
    }
    r += i18n("<tr><td><i>Product ID</i></td><td>0x%1</td></tr>", p);
    r += i18n("<tr><td><i>Revision</i></td><td>%1</td></tr>", _rev);
    r += "<tr><td></td></tr>";

    r += i18n("<tr><td><i>Speed</i></td><td>%1 Mbit/s</td></tr>", _speed);
    r += i18n("<tr><td><i>Channels</i></td><td>%1</td></tr>", _channels);
    r += i18n("<tr><td><i>Max. Packet Size</i></td><td>%1</td></tr>", _maxPacketSize);
    r += "<tr><td></td></tr>";

    r += "</table>";

    return r;
}

bool USBDevice::init() {
    _devices.clear();

    struct libusb_context *libusbctx = nullptr;
    libusb_init(&libusbctx);
    struct libusb_device **libusbdevices = nullptr;
    const size_t libusbdevicessize = libusb_get_device_list(libusbctx, &libusbdevices);
    // qDebug() << Q_FUNC_INFO << libusbdevicessize;
    for (size_t i = 0; i < libusbdevicessize; i++) {
        USBDevice* device = new USBDevice();
        struct libusb_device_descriptor libusbdevice;
        libusb_get_device_descriptor(libusbdevices[i], &libusbdevice);
        // qDebug() << Q_FUNC_INFO << libusbdevice.idVendor << libusbdevice.idProduct;

        device->_bus = libusb_get_bus_number(libusbdevices[i]);
        device->_port = libusb_get_port_number(libusbdevices[i]);
        device->_speed = getSpeed(libusb_get_device_speed(libusbdevices[i]));
        // the maximum is supposed to be 7
        uint8_t portnumbersbuffer[10];
        ::memset(portnumbersbuffer, 0, sizeof(portnumbersbuffer) * sizeof(uint8_t));
        device->_channels = libusb_get_port_numbers(libusbdevices[i], portnumbersbuffer, sizeof(portnumbersbuffer));
        device->_class = libusbdevice.bDeviceClass;
        device->_sub = libusbdevice.bDeviceSubClass;
        device->_prot = libusbdevice.bDeviceProtocol;
        device->_maxPacketSize = libusbdevice.bMaxPacketSize0;
        device->_vendorID = libusbdevice.idVendor;
        device->_prodID = libusbdevice.idProduct;
        if (libusbdevice.iSerialNumber > 0) {
            device->_serial = QString::number(libusbdevice.iSerialNumber);
        }
        device->_ver = getVersion(libusbdevice.bcdUSB);
        device->_rev = getVersion(libusbdevice.bcdDevice);

        device->_device = device->_port;
        device->_level = 0;
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_DRAGONFLY)
        struct libusb_device *libusbparent = libusb_get_parent(libusbdevices[i]);
        if (libusbparent) {
            device->_parent = libusb_get_port_number(libusbparent);
            device->_level = 1;
            struct libusb_device *libusbparentparent = libusb_get_parent(libusbparent);
            if (libusbparentparent) {
                // device->_parent = libusb_get_port_number(libusbparentparent);
                device->_level = 2;
            }
        }
#endif
    }
    libusb_free_device_list(libusbdevices, 1);
    libusb_exit(libusbctx);

    return (libusbdevicessize > 0);
}
