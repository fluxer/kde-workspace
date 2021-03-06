add_subdirectory( sounds  )
########### next target ###############

set(knotify_SRCS
    main.cpp
    knotify.cpp
    notifybysound.cpp
    notifybypopup.cpp
    notifybylogfile.cpp
    notifybytaskbar.cpp
    notifybyexecute.cpp
    notifybyktts.cpp
    imageconverter.cpp
    ksolidnotify.cpp
)

set(knotifyplugin_SRCS
    knotifyplugin.cpp
    knotifyconfig.cpp
)

set(knotifyplugin_HEADERS
    knotifyplugin.h
    knotifyconfig.h
    knotify_export.h
)

qt4_add_dbus_interfaces(knotify_SRCS ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.KSpeech.xml)

set (knotify_OUTPUT_NAME knotify4)
add_executable( knotify ${knotify_SRCS})

add_library( knotifyplugin SHARED ${knotifyplugin_SRCS})

target_link_libraries( knotify ${KDE4_KDEUI_LIBS} ${KDE4_SOLID_LIBS} knotifyplugin kmediaplayer)

target_link_libraries( knotifyplugin ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})


set_target_properties(knotify PROPERTIES OUTPUT_NAME knotify4 )
install(TARGETS knotify ${INSTALL_TARGETS_DEFAULT_ARGS} )
########### install files ###############

install( FILES kde.notifyrc  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kde )
install( FILES hardwarenotifications.notifyrc DESTINATION ${KDE4_DATA_INSTALL_DIR}/hardwarenotifications )
install( FILES knotify4.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

install( FILES knotifynotifymethod.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR} )
install( TARGETS knotifyplugin ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES ${knotifyplugin_HEADERS} DESTINATION ${KDE4_INCLUDE_INSTALL_DIR} )

########### D-Bus Autostart Services #########


configure_file(org.kde.knotify.service.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.knotify.service)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.knotify.service DESTINATION ${KDE4_DBUS_SERVICES_INSTALL_DIR} )
