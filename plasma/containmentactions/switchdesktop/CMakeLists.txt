project(plasma-containmentactions-switchdesktop)

set(switchdesktop_SRCS
    desktop.cpp
)

kde4_add_plugin(plasma_containmentactions_switchdesktop ${switchdesktop_SRCS})
target_link_libraries(plasma_containmentactions_switchdesktop KDE4::plasma KDE4::kio)

install(TARGETS plasma_containmentactions_switchdesktop DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containmentactions-switchdesktop.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
