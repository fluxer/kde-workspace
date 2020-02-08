configure_file(${CMAKE_CURRENT_SOURCE_DIR}/metadata.desktop ${CMAKE_CURRENT_BINARY_DIR}/plasma-dataengine-share-addon-wklej.desktop COPYONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-dataengine-share-addon-wklej.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(FILES metadata.desktop
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/shareprovider/wklej/)

install(DIRECTORY contents
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/shareprovider/wklej)
