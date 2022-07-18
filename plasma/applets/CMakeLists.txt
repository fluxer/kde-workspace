add_subdirectory(icon)
add_subdirectory(lock_logout)
add_subdirectory(panelspacer)
add_subdirectory(analog-clock)
add_subdirectory(batterymonitor)
add_subdirectory(calendar)
add_subdirectory(devicenotifier)
add_subdirectory(digital-clock)
add_subdirectory(quicklaunch)
add_subdirectory(system-monitor)
#notifications
#should compile also on windows? (even if doesn't really make sense)
add_subdirectory(notifications)
add_subdirectory(systemtray)

add_subdirectory(kickoff)
add_subdirectory(sangria-menu)
add_subdirectory(trash)
add_subdirectory(folderview)

#task and windowlist depend on libs/taskmanager
add_subdirectory(tasks)
add_subdirectory(windowlist)
add_subdirectory(pager)
