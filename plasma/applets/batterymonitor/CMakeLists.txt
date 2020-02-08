project(batterymonitor)

install(DIRECTORY contents
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/plasmoids/battery)
install(FILES metadata.desktop
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/plasmoids/battery)

install(FILES metadata.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
        RENAME plasma-applet-batterymonitor.desktop)

install(FILES
        battery-oxygen.svgz
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
