project(fileviewbazaarplugin)

kde4_add_plugin(fileviewbazaarplugin fileviewbazaarplugin.cpp)

target_link_libraries(fileviewbazaarplugin ${KDE4_KIO_LIBS} konq)

install(
    FILES
    fileviewbazaarplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    TARGETS fileviewbazaarplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
