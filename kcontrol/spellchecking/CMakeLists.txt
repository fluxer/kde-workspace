set(kcm_spellchecking_SRCS
    spellchecking.cpp
)

kde4_add_plugin(kcm_spellchecking ${kcm_spellchecking_SRCS})

target_link_libraries(kcm_spellchecking KDE4::kdeui)

install(
    TARGETS kcm_spellchecking
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES kcm_spellchecking.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
