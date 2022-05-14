set(katemailfilesplugin_PART_SRCS
    katemailfiles.cpp
    katemailfilesdialog.cpp
)

kde4_add_plugin(katemailfilesplugin ${katemailfilesplugin_PART_SRCS})


target_link_libraries(katemailfilesplugin
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KPARTS_LIBS}
    kateinterfaces
)

install(
    TARGETS katemailfilesplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katemailfiles
)
install(
    FILES katemailfilesplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
