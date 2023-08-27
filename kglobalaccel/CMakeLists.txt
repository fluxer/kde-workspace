project(kglobalaccel)

###############################################################################
### KDED Global Accel Daemon

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config-kglobalaccel.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kglobalaccel.h
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1219)

set(kglobalaccel_SRCS
    main.cpp
    kglobalacceld.cpp
    component.cpp
    globalshortcut.cpp
    globalshortcutsregistry.cpp
    globalshortcutcontext.cpp
    kglobalaccel_x11.cpp
)

add_executable(kglobalaccel ${kglobalaccel_SRCS})

target_link_libraries(kglobalaccel
    KDE4::kdeui
    KDE4::kio
    ${X11_LIBRARIES}
)

# Install application and configuration
install(
    TARGETS kglobalaccel
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
# install(FILES kglobalaccel.desktop DESTINATION ${KDE4_AUTOSTART_INSTALL_DIR})
install(
    FILES kglobalaccel.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
# KNotify configuration
install(
    FILES kglobalaccel.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)

kde4_add_dbus_service(org.kde.kglobalaccel.service.in)
