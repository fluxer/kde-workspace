add_subdirectory(applicationjobs)
add_subdirectory(apps)
add_subdirectory(devicenotifications)
add_subdirectory(dict)
add_subdirectory(executable)
add_subdirectory(favicons)
add_subdirectory(filebrowser)
add_subdirectory(geolocation)
add_subdirectory(hotplug)
add_subdirectory(keystate)
add_subdirectory(mpris2)
#add_subdirectory(network)
add_subdirectory(notifications)
add_subdirectory(places)
add_subdirectory(powermanagement)
add_subdirectory(soliddevice)
add_subdirectory(time)
add_subdirectory(weather)
if(DBUSMENUQT_FOUND)
    add_subdirectory(statusnotifieritem)
endif()
add_subdirectory(mouse)
add_subdirectory(systemmonitor)
add_subdirectory(tasks)

