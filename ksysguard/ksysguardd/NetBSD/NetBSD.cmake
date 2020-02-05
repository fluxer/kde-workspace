SET(LIBKSYSGUARDD_SOURCES
    NetBSD/apm.c
    NetBSD/CPU.c
    NetBSD/diskstat.c
    NetBSD/loadavg.c
    NetBSD/logfile.c
    NetBSD/Memory.c
    NetBSD/netdev.c
    NetBSD/ProcessList.c
)

SET(LIBKSYSGUARDD_LIBS kvm)
