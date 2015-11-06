kde4_add_plugin(kcm_emoticons emoticonslist.cpp)

target_link_libraries(kcm_emoticons
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KEMOTICONS_LIBRARY}
)

########### install files ###############

install(
    TARGETS kcm_emoticons
    DESTINATION ${PLUGIN_INSTALL_DIR}
)

install(
    FILES emoticons.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
install(
    FILES emoticons.knsrc
    DESTINATION ${CONFIG_INSTALL_DIR}
)
