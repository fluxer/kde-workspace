SET(LIBKSYSGUARDD_SOURCES
    Linux/acpi.c
    Linux/apm.c
    Linux/cpuinfo.c
    Linux/diskstat.c
    Linux/diskstats.c
    Linux/i8k.c
    Linux/loadavg.c
    Linux/logfile.c
    Linux/Memory.c
    Linux/netdev.c
    Linux/netstat.c
    Linux/ProcessList.c
    Linux/stat.c
    Linux/softraid.c
    Linux/uptime.c
)

if(SENSORS_FOUND)
    SET(LIBKSYSGUARDD_SOURCES ${LIBKSYSGUARDD_SOURCES} Linux/lmsensors.c)
endif(SENSORS_FOUND)

if(EXISTS /proc/i8k)
    ADD_DEFINITIONS(-DHAVE_I8K_SUPPORT)
ENDIF(EXISTS /proc/i8k)

if(SENSORS_FOUND)
    SET(LIBKSYSGUARDD_LIBS ${SENSORS_LIBRARIES})
endif(SENSORS_FOUND)

