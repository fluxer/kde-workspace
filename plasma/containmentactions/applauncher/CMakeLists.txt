project(plasma-containmentactions-applauncher)

set(applauncher_SRCS
    launch.cpp
)

kde4_add_plugin(plasma_containmentactions_applauncher ${applauncher_SRCS})
target_link_libraries(plasma_containmentactions_applauncher ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_containmentactions_applauncher DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containmentactions-applauncher.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
