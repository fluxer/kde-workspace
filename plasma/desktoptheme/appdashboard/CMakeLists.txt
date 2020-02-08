
install(FILES metadata.desktop DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/appdashboard/)

FILE(GLOB widgets widgets/*.svgz)
install( FILES ${widgets} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/appdashboard/widgets/ )


