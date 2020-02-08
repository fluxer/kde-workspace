
install(FILES colors metadata.desktop DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/oxygen/)

FILE(GLOB widgets widgets/*.svgz)
install( FILES ${widgets} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/oxygen/widgets/ )

FILE(GLOB dialogs dialogs/*.svgz)
install( FILES ${dialogs} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/oxygen/dialogs/ )

FILE(GLOB opaque opaque/widgets/*.svgz)
install( FILES ${opaque} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/oxygen/opaque/widgets/ )

FILE(GLOB opaque opaque/dialogs/*.svgz)
install( FILES ${opaque} DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/oxygen/opaque/dialogs/ )

