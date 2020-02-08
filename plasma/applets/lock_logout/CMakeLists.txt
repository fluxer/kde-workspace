project(lockout)

install(DIRECTORY contents
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/plasmoids/lockout)
install(FILES metadata.desktop
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/plasmoids/lockout)

install(FILES metadata.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
        RENAME plasma-applet-lockout.desktop)
