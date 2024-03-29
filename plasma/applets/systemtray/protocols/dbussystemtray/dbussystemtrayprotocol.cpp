/***************************************************************************
 *   dbussystemtrayprotocol.cpp                                            *
 *                                                                         *
 *   Copyright (C) 2009 Marco Martin <notmart@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "dbussystemtraytask.h"
#include "dbussystemtrayprotocol.h"

#include <Plasma/DataEngineManager>


namespace SystemTray
{

DBusSystemTrayProtocol::DBusSystemTrayProtocol(QObject *parent)
    : Protocol(parent),
      m_dataEngine(Plasma::DataEngineManager::self()->loadEngine("statusnotifieritem")),
      m_tasks()
{
}

DBusSystemTrayProtocol::~DBusSystemTrayProtocol()
{
    Plasma::DataEngineManager::self()->unloadEngine("statusnotifieritem");
}

void DBusSystemTrayProtocol::init()
{
    if (m_dataEngine->isValid()) {
        initRegisteredServices();
        connect(m_dataEngine, SIGNAL(sourceAdded(QString)),
                this, SLOT(newTask(QString)));
        connect(m_dataEngine, SIGNAL(sourceRemoved(QString)),
                this, SLOT(cleanupTask(QString)));
    }
}

void DBusSystemTrayProtocol::newTask(const QString &service)
{
    if (m_tasks.contains(service)) {
        return;
    }

    DBusSystemTrayTask *task = new DBusSystemTrayTask(service, m_dataEngine, this);

    m_tasks[service] = task;
}

void DBusSystemTrayProtocol::cleanupTask(const QString &service)
{
    DBusSystemTrayTask *task = m_tasks.value(service);

    if (task) {
        m_dataEngine->disconnectSource(service, task);
        m_tasks.remove(service);
        if (task->isValid()) {
            emit task->destroyed(task);
        }
        task->deleteLater();
    }
}

void DBusSystemTrayProtocol::initedTask(DBusSystemTrayTask *task)
{
    emit taskCreated(task);
}

void DBusSystemTrayProtocol::initRegisteredServices()
{
    if (m_dataEngine->isValid()) {
        foreach (const QString &service, m_dataEngine->sources()) {
            newTask(service);
        }
    }
}

}

#include "moc_dbussystemtrayprotocol.cpp"
