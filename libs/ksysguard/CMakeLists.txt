configure_file(config-ksysguard.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ksysguard.h)

########### next target ###############

add_subdirectory( lsofui )
add_subdirectory( processcore )
add_subdirectory( processui )
add_subdirectory( signalplotter )
add_subdirectory( ksgrd )
if(ENABLE_TESTING)
    add_subdirectory( tests )
endif()

install(DIRECTORY scripts/ DESTINATION ${KDE4_DATA_INSTALL_DIR}/ksysguard/scripts)

