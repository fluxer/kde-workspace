set(katebuild_SRCS
    plugin_katebuild.cpp
    targets.cpp
    selecttargetdialog.cpp
)

kde4_add_plugin(katebuildplugin ${katebuild_SRCS})

target_link_libraries(katebuildplugin
    KDE4::kdecore
    kateinterfaces
    KDE4::ktexteditor
)

install(
    TARGETS katebuildplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katebuild
)
install(
    FILES katebuildplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
