add_definitions(-DKDE_DEFAULT_DEBUG_AREA=901)

add_subdirectory(sounds)

########### next target ###############

set(knotify_SRCS
    main.cpp
    knotify.cpp
    notifybysound.cpp
    notifybypopup.cpp
    notifybylogfile.cpp
    notifybytaskbar.cpp
    notifybyexecute.cpp
    imageconverter.cpp
    ksolidnotify.cpp
    knotifyplugin.cpp
    knotifyconfig.cpp
)

add_executable(knotify4 ${knotify_SRCS})

target_link_libraries(knotify4
    KDE4::kdeui
    KDE4::solid
    KDE4::kmediaplayer
)

install(
    TARGETS knotify4
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kde.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kde
)
install(
    FILES hardwarenotifications.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/hardwarenotifications
)
install(
    FILES knotify4.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

########### D-Bus Autostart Services #########

kde4_add_dbus_service(org.kde.knotify.service.in)
