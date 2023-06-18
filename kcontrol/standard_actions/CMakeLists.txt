set(kcm_standard_actions_PART_SRCS standard_actions_module.cpp)

kde4_add_plugin(kcm_standard_actions ${kcm_standard_actions_PART_SRCS})

target_link_libraries(kcm_standard_actions
    KDE4::kdeui
    KDE4::kio
    ${X11_X11_LIB}
)

install(
    TARGETS kcm_standard_actions
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES standard_actions.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

