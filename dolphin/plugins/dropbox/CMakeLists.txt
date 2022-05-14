project(fileviewdropboxplugin)

kde4_add_plugin(fileviewdropboxplugin fileviewdropboxplugin.cpp)

target_link_libraries(fileviewdropboxplugin ${KDE4_KIO_LIBS} konq)

install(
    FILES fileviewdropboxplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    TARGETS fileviewdropboxplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)