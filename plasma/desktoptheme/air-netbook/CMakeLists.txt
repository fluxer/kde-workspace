
install(FILES colors metadata.desktop DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/air-netbook/)

FILE(GLOB widgets widgets/*.svgz)
install( FILES ${widgets} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/air-netbook/widgets/ )

