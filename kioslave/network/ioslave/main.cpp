/*
    This file is part of the network kioslave, part of the KDE project.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkslave.h"

// KDE
#include <KComponentData>
#include <kdemacros.h>
// Qt
#include <QCoreApplication>

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: kio_network app-socket\n");
        exit(-1);
    }

    KComponentData componentData("kio_network");
    QCoreApplication app(argc, argv);

    NetworkSlave slave(argv[1]);
    slave.dispatchLoop();

    return 0;
}
