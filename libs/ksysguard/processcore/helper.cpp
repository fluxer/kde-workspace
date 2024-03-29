/*
    KSysGuard, the KDE System Guard

	Copyright (c) 2009 John Tapsell <john.tapsell@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "helper.h"
#include "processes_local_p.h"

KSysGuardProcessListHelper::KSysGuardProcessListHelper(const char* const helper, QObject *parent)
    : KAuthorization(helper, parent)
{
    qRegisterMetaType<QList<long long> >();
}

/* The functions here run as ROOT.  So be careful.  DO NOT TRUST THE INPUTS TO BE SANE. */
#define GET_PID(i) parameters.value(QString("pid%1").arg(i), -1).toLongLong(); if(pid < 0) return KAuthorization::HelperError;

int KSysGuardProcessListHelper::renice(QVariantMap parameters)
{
    if(!parameters.contains("nicevalue") || !parameters.contains("pidcount"))
        return KAuthorization::HelperError;

    KSysGuard::ProcessesLocal processes;
    int niceValue = parameters.value("nicevalue").toInt();
    bool success = true;
    int numProcesses = parameters.value("pidcount").toInt();
    for (int i = 0; i < numProcesses; ++i) {
        qlonglong pid = GET_PID(i);
        success = processes.setNiceness(pid, niceValue) && success;
    }
    if (success) {
        return KAuthorization::NoError;
    }
    return KAuthorization::HelperError;
}

int KSysGuardProcessListHelper::changecpuscheduler(QVariantMap parameters)
{
    if (!parameters.contains("cpuScheduler") || !parameters.contains("cpuSchedulerPriority") || !parameters.contains("pidcount")) {
        return KAuthorization::HelperError;
    }

    KSysGuard::ProcessesLocal processes;
    int cpuScheduler = parameters.value("cpuScheduler").toInt();
    int cpuSchedulerPriority = parameters.value("cpuSchedulerPriority").toInt();
    bool success = true;

    int numProcesses = parameters.value("pidcount").toInt();
    for (int i = 0; i < numProcesses; ++i) {
        qlonglong pid = GET_PID(i);
        success = processes.setScheduler(pid, cpuScheduler, cpuSchedulerPriority) && success;
    }
    if (success) {
        return KAuthorization::NoError;
    }
    return KAuthorization::HelperError;

}
K_AUTH_MAIN("org.kde.ksysguard.processlisthelper", KSysGuardProcessListHelper)

