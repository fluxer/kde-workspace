add_subdirectory(activitybar)
add_subdirectory(icon)
add_subdirectory(lock_logout)
add_subdirectory(panelspacer)

add_subdirectory(analog-clock)
add_subdirectory(batterymonitor)
add_subdirectory(calendar)
add_subdirectory(devicenotifier)
add_subdirectory(digital-clock)
add_subdirectory(quicklaunch)

if(QT_QTWEBKIT_FOUND)
   add_subdirectory(webbrowser)
endif(QT_QTWEBKIT_FOUND)

add_subdirectory(system-monitor)
#notifications
#should compile also on windows? (even if doesn't really make sense)
add_subdirectory(notifications)
add_subdirectory(systemtray)
