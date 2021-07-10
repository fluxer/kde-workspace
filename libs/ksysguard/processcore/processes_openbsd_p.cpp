/*  This file is part of the KDE project
    Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>

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

#include "processes_local_p.h"
#include "process.h"

#include <klocale.h>

#include <QSet>

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

namespace KSysGuard
{

class ProcessesLocal::Private
{
public:
    Private() {;}
    ~Private() {;}
    inline bool readProc(long pid, struct kinfo_proc *p);
    inline void readProcStatus(struct kinfo_proc *p, Process *process);
    inline void readProcStat(struct kinfo_proc *p, Process *process);
    inline void readProcStatm(struct kinfo_proc *p, Process *process);
    inline bool readProcCmdline(long pid, Process *process);
};

bool ProcessesLocal::Private::readProc(long pid, struct kinfo_proc *p)
{
    int mib[6];
    size_t len;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = pid;
    mib[4] = sizeof(struct kinfo_proc);
    mib[5] = 1;

    len = sizeof(struct kinfo_proc);
    if (sysctl(mib, 6, p, &len, NULL, 0) == -1 || !len)
        return false;
    return true;
}

void ProcessesLocal::Private::readProcStatus(struct kinfo_proc *p, Process *process)
{
    process->setUid(p->p_ruid);
    process->setEuid(p->p_uid);
    process->setSuid(p->p_sid);
    process->setGid(p->p_rgid);
    process->setEgid(p->p_gid);
    process->setName(QString(p->p_comm));
}

void ProcessesLocal::Private::readProcStat(struct kinfo_proc *p, Process *ps)
{
    // TODO: verify
    int status;
    int pagesize = getpagesize();
    ps->setUserTime(p->p_uutime_sec * 100);
    ps->setSysTime(p->p_ustime_sec * 100);
    ps->setNiceLevel(p->p_nice - NZERO);
    ps->setUserUsage(p->p_pctcpu / 100);
    ps->setVmSize((p->p_vm_tsize + p->p_vm_dsize + p->p_vm_ssize + p->p_vm_rssize) * pagesize / 1024);
    ps->setVmRSS(p->p_vm_rssize * pagesize / 1024);
    status = p->p_stat;

    // "idle", "run", "sleep", "stop", "zombie", "dead", "onproc"
    switch( status ) {
        case SIDL:
            ps->setStatus(Process::DiskSleep);
            break;
        case SRUN:
        case SONPROC:
            ps->setStatus(Process::Running);
            break;
        case SSLEEP:
            ps->setStatus(Process::Sleeping);
            break;
        case SSTOP:
            ps->setStatus(Process::Stopped);
            break;
        case SZOMB:
            ps->setStatus(Process::Zombie);
            break;
        case SDEAD:
            ps->setStatus(Process::Ended);
            break;
        default:
            ps->setStatus(Process::OtherStatus);
            break;
    }
}

void ProcessesLocal::Private::readProcStatm(struct kinfo_proc *p, Process *process)
{
    // TODO: verify
    unsigned long shared = p->p_vm_tsize + p->p_vm_dsize + p->p_vm_ssize;
    process->setVmURSS(process->vmRSS - (shared * getpagesize() / 1024));
}

bool ProcessesLocal::Private::readProcCmdline(long pid, Process *process)
{
    int mib[4];
    size_t buflen = 4096;
    char buf[4096];

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC_ARGS;
    mib[2] = pid;
    mib[3] = KERN_PROC_ARGV;

    if (sysctl(mib, 4, buf, &buflen, NULL, 0) == -1 || !buflen) {
        return false;
    }

    QString command;
    char **procargv = (char**) buf;
    for (int i = 0; procargv[i] != NULL; i++) {
        if (i != 0) {
            command.append(QLatin1Char(' '));
        }
        command.append(procargv[i]);
    }
    process->setCommand(command.trimmed());

    return true;
}

ProcessesLocal::ProcessesLocal() : d(new Private())
{
}

long ProcessesLocal::getParentPid(long pid)
{
    Q_ASSERT(pid != 0);
    long ppid = -1;
    struct kinfo_proc p;
    if(d->readProc(pid, &p)) {
        ppid = p.p_ppid;
    }
    return ppid;
}

bool ProcessesLocal::updateProcessInfo( long pid, Process *process)
{
    struct kinfo_proc p;
    if(!d->readProc(pid, &p))
        return false;
    d->readProcStat(&p, process);
    d->readProcStatus(&p, process);
    d->readProcStatm(&p, process);
    if(!d->readProcCmdline(pid, process))
        return false;

    return true;
}

QSet<long> ProcessesLocal::getAllPids( )
{
    QSet<long> pids;
    int mib[6];
    size_t len;
    size_t num;
    struct kinfo_proc *p;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;
    mib[3] = 0;
    mib[4] = sizeof(struct kinfo_proc);
    mib[5] = 0;
    if (sysctl(mib, 6, NULL, &len, NULL, 0) == -1 || !len) {
        return pids;
    }
    p = (kinfo_proc *) malloc(len);
    mib[5] = len / sizeof(struct kinfo_proc);
    if (sysctl(mib, 6, p, &len, NULL, 0) == -1) {
        free(p);
        return pids;
    }

    for (num = 0; num < len / sizeof(struct kinfo_proc); num++) {
        pids.insert(p[num].p_pid);
    }
    free(p);
    return pids;
}

bool ProcessesLocal::sendSignal(long pid, int sig)
{
    if ( kill( (pid_t)pid, sig ) ) {
        // Kill failed
        return false;
    }
    return true;
}

bool ProcessesLocal::setNiceness(long pid, int priority)
{
    if ( setpriority( PRIO_PROCESS, pid, priority ) ) {
        // set niceness failed
        return false;
    }
    return true;
}

bool ProcessesLocal::setScheduler(long pid, int priorityClass, int priority)
{
    if (priorityClass == KSysGuard::Process::Other || priorityClass == KSysGuard::Process::Batch)
        priority = 0;
    if (pid <= 0) return false; // check the parameters
        return false;
    // not supported by OpenBSD yet (last checked on 6.8)
#if 0
    struct sched_param params;
    params.sched_priority = priority;
    switch(priorityClass) {
      case (KSysGuard::Process::Other):
            return (sched_setscheduler( pid, SCHED_OTHER, &params) == 0);
      case (KSysGuard::Process::RoundRobin):
            return (sched_setscheduler( pid, SCHED_RR, &params) == 0);
      case (KSysGuard::Process::Fifo):
            return (sched_setscheduler( pid, SCHED_FIFO, &params) == 0);
#ifdef SCHED_BATCH
      case (KSysGuard::Process::Batch):
            return (sched_setscheduler( pid, SCHED_BATCH, &params) == 0);
#endif
      default:
            return false;
    }
#else
    return false;
#endif
}

long long ProcessesLocal::totalPhysicalMemory()
{
    static int physmem_mib[] = { CTL_HW, HW_PHYSMEM };
    /* get the page size with "getpagesize" and calculate pageshift from it */
    int pagesize = ::getpagesize();
    int pageshift = 0;
    while (pagesize > 1) {
        pageshift++;
        pagesize >>= 1;
    }
    size_t Total = 0;
    size_t size = sizeof(Total);
    sysctl(physmem_mib, 2, &Total, &size, NULL, 0);
    return Total /= 1024;
}

#ifndef _SC_NPROCESSORS_ONLN
long int KSysGuard::ProcessesLocal::numberProcessorCores()
{
    int mib[2];
    int ncpu;
    size_t len;

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(ncpu);

    if (sysctl(mib, 2, &ncpu, &len, NULL, 0) == -1 || !len)
        return 1;
    return len;
}
#endif

ProcessesLocal::~ProcessesLocal()
{
   delete d;
}

}
