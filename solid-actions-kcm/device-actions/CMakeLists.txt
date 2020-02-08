FILE(GLOB desktopfile *.desktop)

INSTALL( FILES ${desktopfile} DESTINATION ${KDE4_DATA_INSTALL_DIR}/solid/devices )
