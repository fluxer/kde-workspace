project(autostart)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

########### next target ###############

set(kcm_autostart_PART_SRCS
   autostartitem.cpp
   addscriptdialog.cpp
   advanceddialog.cpp
   autostart.cpp
)

kde4_add_plugin(kcm_autostart ${kcm_autostart_PART_SRCS})

target_link_libraries(kcm_autostart ${KDE4_KIO_LIBS})

########### install files ###############

install(
    TARGETS kcm_autostart
    DESTINATION ${PLUGIN_INSTALL_DIR}
)
install(
    FILES autostart.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
