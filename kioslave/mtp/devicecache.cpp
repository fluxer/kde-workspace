/*
    Cache for recently used devices.
    Copyright (C) 2012  Philipp Schmidt <philschmidt@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "devicecache.h"
#include "kio_mtp_helpers.h"

// #include <libudev.h>
// #include <fcntl.h>

#include <Solid/Device>
#include <Solid/PortableMediaPlayer>
#include <Solid/DeviceNotifier>

static const QString solidMTPProtocol = QLatin1String("mtp");

/**
 * Creates a Cached Device that has a predefined lifetime (default: 10000 msec)s
 * The lifetime is reset every time the device is accessed. After it expires it
 * will be released.
 *
 * @param device The LIBMTP_mtpdevice_t pointer to cache
 * @param udi The UDI of the new device to cache
 */
CachedDevice::CachedDevice(LIBMTP_mtpdevice_t* device, LIBMTP_raw_device_t* rawdevice, const QString udi, qint32 timeout)
{
    this->timeout = timeout;
    this->mtpdevice = device;
    this->rawdevice = *rawdevice;
    this->udi = udi;

    char *deviceName = LIBMTP_Get_Friendlyname(device);
    char *deviceModel = LIBMTP_Get_Modelname(device);

    // prefer friendly devicename over model
    if (!deviceName) {
        name = QString::fromUtf8(deviceModel);
    } else {
        name = QString::fromUtf8(deviceName);
    }

    kDebug(KIO_MTP) << "Created device" << name << "with udi" << udi << " and timeout" << timeout;
}

CachedDevice::~CachedDevice()
{
    LIBMTP_Release_Device(mtpdevice);
}

LIBMTP_mtpdevice_t* CachedDevice::getDevice()
{
    if (!mtpdevice->storage) {
        kDebug(KIO_MTP) << "reopen mtpdevice if we have no storage found";
        LIBMTP_Release_Device(mtpdevice);
        mtpdevice = LIBMTP_Open_Raw_Device_Uncached(&rawdevice);
        resetDeviceStack(mtpdevice);
    }
    return mtpdevice;
}

const QString CachedDevice::getName() const
{
    return name;
}

const QString CachedDevice::getUdi() const
{
    return udi;
}

DeviceCache::DeviceCache(qint32 timeout, QObject*parent)
    : QEventLoop(parent)
{
    this->timeout = timeout;
    
    notifier = Solid::DeviceNotifier::instance();
    
    connect(notifier, SIGNAL(deviceAdded(QString)), this, SLOT(deviceAdded(QString)));
    connect(notifier, SIGNAL(deviceRemoved(QString)), this, SLOT(deviceRemoved(QString)));
    
    foreach (Solid::Device solidDevice, Solid::Device::listFromType(Solid::DeviceInterface::PortableMediaPlayer)) {
        checkDevice(solidDevice);
    }
}

void DeviceCache::checkDevice(Solid::Device solidDevice)
{
    Solid::PortableMediaPlayer *iface = solidDevice.as<Solid::PortableMediaPlayer>();
    if (!iface) {
        kWarning(KIO_MTP) << "Not portable media player device" << solidDevice.udi();
        return;
    } else if (!iface->supportedProtocols().contains(solidMTPProtocol)) {
        kDebug(KIO_MTP) << "Not MTP device" << solidDevice.udi();
        return;
    }

    const QByteArray solidSerial = iface->driverHandle(solidMTPProtocol).toByteArray();
    if (solidSerial.isEmpty()) {
        kWarning(KIO_MTP) << "No serial for device" << solidDevice.udi();
        return;
    }

    if (!udiCache.contains(solidDevice.udi())) {
        kDebug(KIO_MTP) << "New device, getting raw devices";

        LIBMTP_raw_device_t *rawdevices = 0;
        int numrawdevices;

        LIBMTP_error_number_t err = LIBMTP_Detect_Raw_Devices(&rawdevices, &numrawdevices);
        switch (err) {
            case LIBMTP_ERROR_CONNECTING: {
                kError(KIO_MTP) << "There has been an error connecting to the devices";
                break;
            }
            case LIBMTP_ERROR_MEMORY_ALLOCATION: {
                kError(KIO_MTP) << "Encountered a Memory Allocation Error";
                break;
            }
            case LIBMTP_ERROR_NONE: {
                kDebug(KIO_MTP) << "No Error, continuing";

                for (int i = 0; i < numrawdevices; i++) {
                    LIBMTP_raw_device_t* rawDevice = &rawdevices[i];

                    LIBMTP_mtpdevice_t *mtpDevice = LIBMTP_Open_Raw_Device_Uncached(rawDevice);
                    resetDeviceStack(mtpDevice);
                    const char* rawDeviceSerial = LIBMTP_Get_Serialnumber(mtpDevice);

                    kDebug(KIO_MTP) << "Checking for device match" << solidSerial << rawDeviceSerial;
                    if (solidSerial == rawDeviceSerial) {
                        kDebug( KIO_MTP ) << "Found device matching the Solid description";
                    } else {
                        LIBMTP_Release_Device(mtpDevice);
                        continue;
                    }

                    if (!udiCache.contains(solidDevice.udi())) {
                        CachedDevice *cDev = new CachedDevice(mtpDevice, rawDevice, solidDevice.udi(), timeout);
                        udiCache.insert(solidDevice.udi(), cDev);
                        nameCache.insert(cDev->getName(), cDev);
                    }
                }
                break;
            }
            case LIBMTP_ERROR_GENERAL:
            default: {
                kError(KIO_MTP) << "Unknown connection error";
                break;
            }
        }
        ::free(rawdevices);
    }
}

void DeviceCache::deviceAdded(const QString& udi)
{
    kDebug(KIO_MTP) << "New device attached with udi" << udi << ". Checking if PortableMediaPlayer...";

    Solid::Device device(udi);
    if (device.isDeviceInterface(Solid::DeviceInterface::PortableMediaPlayer)) {
        kDebug(KIO_MTP) << "SOLID: New Device with udi" << udi;

        checkDevice(device);
    }
}

void DeviceCache::deviceRemoved(const QString& udi)
{
    if (udiCache.contains(udi)) {
        kDebug(KIO_MTP) << "SOLID: Device with udi" << udi << "removed.";
        
        CachedDevice *cDev = udiCache.value(udi);
        
        udiCache.remove(cDev->getUdi());
        nameCache.remove(cDev->getName());
        delete cDev;
    }
}

DeviceCache::~DeviceCache()
{
    processEvents();
    
    // Release devices
    foreach (QString udi, udiCache.keys()) {
        deviceRemoved(udi);
    }
}

QHash<QString, CachedDevice*> DeviceCache::getAll()
{
    kDebug(KIO_MTP ) << "getAll()";

    processEvents();

    return nameCache;
}

bool DeviceCache::contains(QString string, bool isUdi)
{
    processEvents();

    if (isUdi) {
        return udiCache.contains(string);
    }
    return nameCache.contains(string);
}

CachedDevice* DeviceCache::get(const QString& string, bool isUdi)
{
    processEvents();

    if (isUdi) {
        return udiCache.value(string);
    }
    return nameCache.value(string);
}

int DeviceCache::size()
{
    processEvents();
    
    return nameCache.size();
}

#include "moc_devicecache.cpp"
