project(plasma-applet-lockout)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/kworkspace
)

set(lockout_SRCS
    lockout.cpp
)

kde4_add_plugin(plasma_applet_lockout ${lockout_SRCS})
target_link_libraries(plasma_applet_lockout
    KDE4::plasma
    KDE4::solid
    kworkspace
)

install(
    TARGETS plasma_applet_lockout
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-lockout.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
    RENAME plasma-applet-lockout.desktop
)
