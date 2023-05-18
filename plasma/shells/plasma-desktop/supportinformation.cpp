/*
 * Class to generate support information for plasma shells
 *
 * Copyright (C) 2013 David Edmundson <kde@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "supportinformation.h"

#include <QBuffer>
#include <Plasma/Applet>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/Package>
#include <Plasma/PackageMetadata>

// this is deliberately _not_ in i18n, the information is for uploading to a bug report so should always be in
// English so as to be useful for developers

QString SupportInformation::generateSupportInformation(Plasma::Corona *corona)
{
    QBuffer infoBuffer;
    infoBuffer.open(QIODevice::WriteOnly);

    {
        QDebug stream(&infoBuffer);
        SupportInformation info(stream);

        info.addHeader();
        info.addInformationForCorona(corona);
    }

    const QByteArray infoData = infoBuffer.data();
    return QString::fromAscii(infoData.constData(), infoData.size());
}

SupportInformation::SupportInformation(const QDebug &outputStream) :
    m_stream(outputStream)
{
}

void SupportInformation::addHeader()
{
    m_stream << "Plasma-desktop Support Information:\n"
             << "The following information should be used when requesting support.\n"
             << "It provides information about the currently running instance and which applets are used.\n"
             << "Please include the information provided underneath this introductory text along with "
             << "whatever you think may be relevant to the issue.\n\n";

    m_stream << "Version\n";
    m_stream << "=======\n";
    m_stream << "KDE SC version (runtime):\n";
    m_stream << KDE::versionString() << '\n';
    m_stream << "KDE SC version (compile):\n";
    m_stream << KDE_VERSION_STRING << '\n';
    m_stream << "Katie Version:\n";
    m_stream << qVersion() << '\n';

    addSeperator();
}

void SupportInformation::addInformationForCorona(Plasma::Corona *corona)
{
    foreach (Plasma::Containment *containment, corona->containments()) {
        addInformationForContainment(containment);
    }
}

void SupportInformation::addInformationForContainment(Plasma::Containment *containment)
{
    // a containment is also an applet so print standard applet information out
    addInformationForApplet(containment);

    foreach (Plasma::Applet *applet, containment->applets()) {
        addInformationForApplet(applet);
    }
}

void SupportInformation::addInformationForApplet(Plasma::Applet *applet)
{
    if (applet->isContainment()) {
        m_stream << "Containment - ";
    } else {
        m_stream << "Applet - ";
    }
    m_stream << applet->name() << ":\n";

    m_stream << "Plugin Name: " << applet->pluginName() << '\n';
    m_stream << "Category: " << applet->category() << '\n';


    if (applet->package()) {
        m_stream << "API: " << applet->package()->metadata().implementationApi() << '\n';
        m_stream << "Type: " << applet->package()->metadata().type() << '\n';
        m_stream << "Version: " << applet->package()->metadata().version() << '\n';
        m_stream << "Author: " << applet->package()->metadata().author() << '\n';
    }

    // runtime info
    m_stream << "Failed To Launch: " << applet->hasFailedToLaunch() << '\n';
    m_stream << "ScreenRect: " << applet->screenRect() << '\n';
    m_stream << "FormFactor: " << applet->formFactor() << '\n';

    m_stream << "Config Group Name: " << applet->config().name() << '\n';

    m_stream << '\n'; // insert a blank line
}

void SupportInformation::addSeperator()
{
    m_stream << '\n' << "=========" << '\n';
}

