/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "solid-hardware.h"

#include <QString>
#include <QStringList>
#include <QMetaProperty>
#include <QMetaEnum>

#include <kcomponentdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>

#include <solid/device.h>
#include <solid/storageaccess.h>
#include <solid/opticaldrive.h>
#include <solid/devicenotifier.h>

#include <iostream>

static const char appName[] = "solid-hardware";
static const char programName[] = I18N_NOOP("solid-hardware");

static const char description[] = I18N_NOOP("KDE tool for querying your hardware from the command line");

static const char version[] = "0.1a";

std::ostream &operator<<(std::ostream &out, const QString &msg)
{
    return (out << msg.toLocal8Bit().constData());
}

std::ostream &operator<<(std::ostream &out, const QVariant &value)
{
    switch (value.type())
    {
    case QVariant::StringList:
    {
        out << "{";

        const QStringList list = value.toStringList();

        QStringList::ConstIterator it = list.constBegin();
        QStringList::ConstIterator end = list.constEnd();

        for (; it!=end; ++it)
        {
            out << "'" << *it << "'";

            if (it+1!=end)
            {
                out << ", ";
            }
        }

        out << "}  (string list)";
        break;
    }
    case QVariant::Bool:
        out << (value.toBool()?"true":"false") << "  (bool)";
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        out << value.toString()
            << "  (0x" << QString::number(value.toLongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        out << value.toString()
            << "  (0x" << QString::number(value.toULongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
        break;        
    case QVariant::UserType:
    {
        //qDebug() << "got variant type:" << value.typeName();
        if (value.canConvert<QList<int> >())
        {
            QList<int> intlist = value.value<QList<int> >();
            QStringList tmp;
            foreach (int val, intlist)
                tmp.append(QString::number(val));
            out << "{" << tmp.join(",") << "} (int list)";
        }
        break;
    }
    default:
        out << "'" << value.toString() << "'  (string)";
        break;
    }

    return out;
}

std::ostream &operator<<(std::ostream &out, const Solid::Device &device)
{
    out << "  parent = " << QVariant(device.parentUdi()) << std::endl;
    out << "  vendor = " << QVariant(device.vendor()) << std::endl;
    out << "  product = " << QVariant(device.product()) << std::endl;
    out << "  description = " << QVariant(device.description()) << std::endl;

    int index = Solid::DeviceInterface::staticMetaObject.indexOfEnumerator("Type");
    QMetaEnum typeEnum = Solid::DeviceInterface::staticMetaObject.enumerator(index);

    for (int i=0; i<typeEnum.keyCount(); i++)
    {
        Solid::DeviceInterface::Type type = (Solid::DeviceInterface::Type)typeEnum.value(i);
        const Solid::DeviceInterface *interface = device.asDeviceInterface(type);

        if (interface)
        {
            const QMetaObject *meta = interface->metaObject();

            for (int i=meta->propertyOffset(); i<meta->propertyCount(); i++)
            {
                QMetaProperty property = meta->property(i);
                out << "  " << QString(meta->className()).mid(7) << "." << property.name()
                    << " = ";

                QVariant value = property.read(interface);

                if (property.isEnumType()) {
                    QMetaEnum metaEnum = property.enumerator();
                    if (metaEnum.isFlag()) {
                        out << "'" << metaEnum.valueToKeys(value.toInt()).constData() << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (flag)";
                    } else {
                        out << "'" << metaEnum.valueToKey(value.toInt()) << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (enum)";
                    }
                    out << std::endl;
                } else {
                    out << value << std::endl;
                }
            }
        }
    }

    return out;
}

std::ostream &operator<<(std::ostream &out, const QMap<QString,QVariant> &properties)
{
    foreach (const QString &key, properties.keys())
    {
        out << "  " << key << " = " << properties[key] << std::endl;
    }

    return out;
}

void checkArgumentCount(int min, int max)
{
    int count = KCmdLineArgs::parsedArgs()->count();

    if (count < min)
    {
        KCmdLineArgs::usageError(i18n("Syntax Error: Not enough arguments"));
    }

    if ((max > 0) && (count > max))
    {
        KCmdLineArgs::usageError(i18n("Syntax Error: Too many arguments"));
    }
}

int main(int argc, char **argv)
{
  KCmdLineArgs::init(argc, argv, appName, 0, ki18n(programName), version, ki18n(description), KCmdLineArgs::CmdLineArgNone);


  KCmdLineOptions options;

  options.add("commands", ki18n("Show available commands"));

  options.add("+command", ki18n("Command (see --commands)"));

  options.add("+[arg(s)]", ki18n("Arguments for command"));

  KCmdLineArgs::addCmdLineOptions(options);

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KComponentData componentData(appName);

  if (args->isSet("commands"))
  {
      KCmdLineArgs::enable_i18n();

      std::cout << std::endl << i18n("Syntax:") << std::endl << std::endl;

      std::cout << "  solid-hardware list [details]" << std::endl;
      std::cout << i18n("             # List the hardware available in the system.\n"
                    "             # - If the 'details' option is specified, the device interfaces\n"
                    "             # and the corresponding properties are listed in a platform\n"
                    "             # neutral fashion,\n"
                    "             # - Otherwise only device UDIs are listed.\n") << std::endl;

      std::cout << "  solid-hardware details 'udi'" << std::endl;
      std::cout << i18n("             # Display all the interfaces and properties of the device\n"
                    "             # corresponding to 'udi' in a platform neutral fashion.\n") << std::endl;

      std::cout << "  solid-hardware 'udi'" << std::endl;
      std::cout << i18n("             # Display all the properties of the device corresponding to 'udi'.\n") << std::endl;

      std::cout << "  solid-hardware query 'predicate' ['parentUdi']" << std::endl;
      std::cout << i18n("             # List the UDI of devices corresponding to 'predicate'.\n"
                    "             # - If 'parentUdi' is specified, the search is restricted to the\n"
                    "             # branch of the corresponding device,\n"
                    "             # - Otherwise the search is done on all the devices.\n") << std::endl;

      std::cout << "  solid-hardware mount 'udi'" << std::endl;
      std::cout << i18n("             # If applicable, mount the device corresponding to 'udi'.\n") << std::endl;

      std::cout << "  solid-hardware unmount 'udi'" << std::endl;
      std::cout << i18n("             # If applicable, unmount the device corresponding to 'udi'.\n") << std::endl;

      std::cout << "  solid-hardware eject 'udi'" << std::endl;
      std::cout << i18n("             # If applicable, eject the device corresponding to 'udi'.\n") << std::endl;

      std::cout << "  solid-hardware listen" << std::endl;
      std::cout << i18n("             # Listen to all add/remove events on supported hardware.") << std::endl;

      return 0;
  }

  return SolidHardware::doIt() ? 0 : 1;
}

bool SolidHardware::doIt()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    checkArgumentCount(1, 0);

    QString command(args->arg(0));

    int fake_argc = 0;
    char **fake_argv = 0;
    SolidHardware shell(fake_argc, fake_argv);

    if (command == "list")
    {
        checkArgumentCount(1, 2);
        QByteArray extra(args->count()==2 ? args->arg(1).toLocal8Bit() : "");
        return shell.hwList(extra=="details");
    }
    else if (command == "details")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwCapabilities(udi);
    }
    else if (command == "query")
    {
        checkArgumentCount(2, 3);

        QString query = args->arg(1);
        QString parent;

        if (args->count() == 3)
        {
            parent = args->arg(2);
        }

        return shell.hwQuery(parent, query);
    }
    else if (command == "mount")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Mount, udi);
    }
    else if (command == "unmount")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Unmount, udi);
    }
    else if (command == "eject")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Eject, udi);
    }
    else if (command == "listen")
    {
        return shell.listen();
    }
    else
    {
        std::cerr << i18n("Syntax Error: Unknown command '%1'" ,command) << std::endl;
    }

    return false;
}

bool SolidHardware::hwList(bool interfaces)
{
    const QList<Solid::Device> all = Solid::Device::allDevices();

    foreach (const Solid::Device &device, all)
    {
        std::cout << "udi = '" << device.udi() << "'" << std::endl;

        if (interfaces)
        {
            std::cout << device << std::endl;
        }
    }

    return true;
}

bool SolidHardware::hwCapabilities(const QString &udi)
{
    const Solid::Device device(udi);

    std::cout << "udi = '" << device.udi() << "'" << std::endl;
    std::cout << device << std::endl;

    return true;
}

bool SolidHardware::hwQuery(const QString &parentUdi, const QString &query)
{
    const QList<Solid::Device> devices
        = Solid::Device::listFromQuery(query, parentUdi);

    foreach (const Solid::Device &device, devices)
    {
        std::cout << "udi = '" << device.udi() << "'" << std::endl;
    }

    return true;
}

bool SolidHardware::hwVolumeCall(SolidHardware::VolumeCallType type, const QString &udi)
{
    Solid::Device device(udi);

    if (!device.is<Solid::StorageAccess>() && type!=Eject)
    {
        std::cerr << i18n("Error: %1 does not have the interface StorageAccess." , udi) << std::endl;
        return false;
    }
    else if (!device.is<Solid::OpticalDrive>() && type==Eject)
    {
        std::cerr << i18n("Error: %1 does not have the interface OpticalDrive." , udi) << std::endl;
        return false;
    }

    switch(type)
    {
    case Mount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(setupDone(Solid::ErrorType, const QString &, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, const QString &, const QString &)));
        device.as<Solid::StorageAccess>()->setup();
        break;
    case Unmount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(teardownDone(Solid::ErrorType, const QString &, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, const QString &, const QString &)));
        device.as<Solid::StorageAccess>()->teardown();
        break;
    case Eject:
        connect(device.as<Solid::OpticalDrive>(),
                SIGNAL(ejectDone(Solid::ErrorType, const QString &, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, const QString &, const QString &)));
        device.as<Solid::OpticalDrive>()->eject();
        break;
    }

    m_loop.exec();

    if (m_error)
    {
        std::cerr << i18n("Error: %1" , m_errorString) << std::endl;
        return false;
    }

    return true;
}

bool SolidHardware::listen()
{
    Solid::DeviceNotifier *notifier = Solid::DeviceNotifier::instance();
    bool a = connect(notifier, SIGNAL(deviceAdded(QString)), this, SLOT(deviceAdded(QString)));
    bool d = connect(notifier, SIGNAL(deviceRemoved(QString)), this, SLOT(deviceRemoved(QString)));

    if (!a || !d) {
        return false;
    }

    std::cout << "Listening to add/remove events: " << std::endl;
    m_loop.exec();
    return true;
}

void SolidHardware::deviceAdded(const QString &udi)
{
    std::cout << "Device Added:" << std::endl;
    std::cout << "udi = '" << udi << "'" << std::endl;
}

void SolidHardware::deviceRemoved(const QString &udi)
{
    std::cout << "Device Removed:" << std::endl;
    std::cout << "udi = '" << udi << "'" << std::endl;
}

void SolidHardware::slotStorageResult(Solid::ErrorType error, const QString &errorData, const QString &udi)
{
    Q_UNUSED(udi);
    if (error) {
        m_error = 1;
        m_errorString = errorData;
    }
    m_loop.exit();
}

#include "moc_solid-hardware.cpp"
