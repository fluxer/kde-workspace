kde4_add_plugin(katetabifyplugin tabify.cpp)

target_link_libraries(katetabifyplugin
    KDE4::kdeui
    KDE4::kparts
    kateinterfaces
)

install(
    TARGETS katetabifyplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

####################### install files #########################################
install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katetabify
)
install(
    FILES katetabifyplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
