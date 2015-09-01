add_subdirectory( qml )
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

# Install the KWin/WindowSwitcher service type
install( FILES kwinwindowswitcher.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
install( FILES kwindesktopswitcher.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
