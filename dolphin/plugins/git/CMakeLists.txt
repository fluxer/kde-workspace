project(fileviewgitplugin)

set(fileviewgitplugin_SRCS
    fileviewgitplugin.cpp
    checkoutdialog.cpp
    commitdialog.cpp
    tagdialog.cpp
    pushdialog.cpp
    pulldialog.cpp
    gitwrapper.cpp
)

kde4_add_kcfg_files(fileviewgitplugin_SRCS fileviewgitpluginsettings.kcfgc)

kde4_add_plugin(fileviewgitplugin  ${fileviewgitplugin_SRCS})

target_link_libraries(fileviewgitplugin ${KDE4_KIO_LIBS} konq)

install(
    FILES fileviewgitplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES fileviewgitpluginsettings.kcfg
    DESTINATION ${KDE4_KCFG_INSTALL_DIR}
)

install(
    TARGETS fileviewgitplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
