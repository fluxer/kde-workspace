
set(icon_SRCS
    icon.cpp)

kde4_add_plugin(plasma_applet_icon ${icon_SRCS})
target_link_libraries(plasma_applet_icon KDE4::plasma KDE4::kio)

install(TARGETS plasma_applet_icon DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-icon.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
