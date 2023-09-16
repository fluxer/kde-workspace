project(batterymonitor)

set(batterymonitor_SRCS
    batterymonitor.cpp
)

kde4_add_plugin(plasma_applet_battery ${batterymonitor_SRCS})
target_link_libraries(plasma_applet_battery
    KDE4::plasma
    KDE4::solid
)

install(
    TARGETS plasma_applet_battery
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-batterymonitor.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES battery-oxygen.svgz
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/default/widgets/
)
