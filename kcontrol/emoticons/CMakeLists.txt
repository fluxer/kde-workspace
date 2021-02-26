kde4_add_plugin(kcm_emoticons emoticonslist.cpp)

target_link_libraries(kcm_emoticons
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KEMOTICONS_LIBS}
)

########### install files ###############

install(
    TARGETS kcm_emoticons
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES emoticons.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
