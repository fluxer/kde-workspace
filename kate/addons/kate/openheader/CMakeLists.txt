########### next target ###############

kde4_add_plugin(kateopenheaderplugin plugin_kateopenheader.cpp)

target_link_libraries(kateopenheaderplugin
    kateinterfaces
    ${KDE4_KIO_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
)

install(
    TARGETS kateopenheaderplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/kateopenheader
)
install(
    FILES kateopenheader.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
