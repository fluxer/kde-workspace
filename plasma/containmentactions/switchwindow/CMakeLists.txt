project(plasma-containmentactions-switchwindow)

set(switchwindow_SRCS
    switch.cpp
    config.ui
)

kde4_add_plugin(plasma_containmentactions_switchwindow ${switchwindow_SRCS})
target_link_libraries(plasma_containmentactions_switchwindow
    KDE4::plasma
    KDE4::kio
)

install(
    TARGETS plasma_containmentactions_switchwindow
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-containmentactions-switchwindow.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
