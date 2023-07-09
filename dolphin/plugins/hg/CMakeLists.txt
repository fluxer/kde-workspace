project(fileviewhgplugin)

set(fileviewhgplugin_SRCS
    fileviewhgplugin.cpp
    renamedialog.cpp
    commitdialog.cpp
    hgwrapper.cpp
    statuslist.cpp
    branchdialog.cpp
    tagdialog.cpp
    updatedialog.cpp
    clonedialog.cpp
    createdialog.cpp
    syncdialogbase.cpp
    mergedialog.cpp
    pushdialog.cpp
    pulldialog.cpp
    hgconfig.cpp
    configdialog.cpp
    commititemdelegate.cpp
    commitinfowidget.cpp
    pathselector.cpp
    bundledialog.cpp
    exportdialog.cpp
    importdialog.cpp
    servedialog.cpp
    servewrapper.cpp
    backoutdialog.cpp
    config-widgets/generalconfig.cpp
    config-widgets/pathconfig.cpp
    config-widgets/ignorewidget.cpp
    config-widgets/pluginsettings.cpp
)

kde4_add_kcfg_files(fileviewhgplugin_SRCS fileviewhgpluginsettings.kcfgc)

kde4_add_plugin(fileviewhgplugin ${fileviewhgplugin_SRCS})

target_link_libraries(fileviewhgplugin KDE4::kio KDE4::ktexteditor konq)

install(
    FILES fileviewhgplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    TARGETS fileviewhgplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
