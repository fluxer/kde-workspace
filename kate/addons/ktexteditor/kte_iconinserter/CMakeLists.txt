kde4_add_plugin(ktexteditor_iconinserter iconinserterplugin.cpp)

target_link_libraries(ktexteditor_iconinserter
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
)

install(
    TARGETS ktexteditor_iconinserter
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES ktexteditor_iconinserter.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES ktexteditor_iconinserterui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/ktexteditor_iconinserter
)
