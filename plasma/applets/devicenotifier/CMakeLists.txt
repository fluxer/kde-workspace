project(plasma-applet-devicenotifier)

set(devicenotifier_SRCS
    devicenotifier.cpp
)

kde4_add_plugin(plasma_applet_devicenotifier ${devicenotifier_SRCS})
target_link_libraries(plasma_applet_devicenotifier
    KDE4::plasma
    KDE4::solid
    KDE4::kio
)

install(
    TARGETS plasma_applet_devicenotifier
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-devicenotifier.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
