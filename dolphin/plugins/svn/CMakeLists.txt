project(fileviewsvnplugin)

set(fileviewsvnplugin_SRCS fileviewsvnplugin.cpp)

kde4_add_kcfg_files(fileviewsvnplugin_SRCS fileviewsvnpluginsettings.kcfgc)

kde4_add_plugin(fileviewsvnplugin  ${fileviewsvnplugin_SRCS})

target_link_libraries(fileviewsvnplugin KDE4::kio konq)

install(
    FILES fileviewsvnplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES fileviewsvnpluginsettings.kcfg
    DESTINATION ${KDE4_KCFG_INSTALL_DIR}
)

install(
    TARGETS fileviewsvnplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
