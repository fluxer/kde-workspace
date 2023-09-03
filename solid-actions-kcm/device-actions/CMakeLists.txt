file(GLOB desktopfile *.desktop)

install(
    FILES ${desktopfile}
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/solid/devices
)
