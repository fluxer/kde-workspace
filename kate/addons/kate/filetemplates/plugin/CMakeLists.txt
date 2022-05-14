########### next target ###############

kde4_add_plugin(katefiletemplates filetemplates.cpp)

target_link_libraries(katefiletemplates
    kateinterfaces
    ${KDE4_KTEXTEDITOR_LIBS}
)

install(
    TARGETS katefiletemplates
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katefiletemplates
)
install(
    FILES katetemplate.xml
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/katepart/syntax
)
install(
    FILES katefiletemplates.desktop
    DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}
)
