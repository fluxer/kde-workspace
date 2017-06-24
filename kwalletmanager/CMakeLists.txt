project(kwallet)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    find_package(KDE4)
    include( KDE4Defaults )
    include_directories(${KDE4_INCLUDES})

    add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
endif()

add_subdirectory( icons )
add_subdirectory( src )


########### install files ###############

install( PROGRAMS kwalletmanager.desktop kwalletmanager-kwalletd.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
