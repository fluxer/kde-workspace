SET(kcm_spellchecking_SRCS
   spellchecking.cpp
)

KDE4_ADD_PLUGIN(kcm_spellchecking ${kcm_spellchecking_SRCS})

TARGET_LINK_LIBRARIES(kcm_spellchecking KDE4::kdeui)

INSTALL(TARGETS
    kcm_spellchecking
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
INSTALL(
    FILES kcm_spellchecking.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
