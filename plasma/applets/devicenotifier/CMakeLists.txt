project(devicenotifier)

install(DIRECTORY package/
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/plasmoids/notifier)

install(FILES package/metadata.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
        RENAME plasma-applet-devicenotifier.desktop)

install(FILES test-predicate-openinwindow.desktop DESTINATION ${KDE4_DATA_INSTALL_DIR}/solid/actions )
