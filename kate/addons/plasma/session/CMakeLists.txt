project(katesessionapplet)

kde4_add_plugin(plasma_applet_katesession katesessionapplet.cpp)

target_link_libraries(plasma_applet_katesession
    KDE4::plasma
    KDE4::kio
)

install(
    TARGETS plasma_applet_katesession
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-applet-katesession.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
