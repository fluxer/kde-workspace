########### next target ###############

kde4_add_plugin(katekonsoleplugin kateconsole.cpp)

target_link_libraries(katekonsoleplugin
    KDE4::kdeui
    KDE4::kparts
    kateinterfaces
)

install(
    TARGETS katekonsoleplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)


########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katekonsole
)
install(
    FILES katekonsoleplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
