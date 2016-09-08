#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/logout/logout.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/logout/logoutconfig.kcfgc)

# .desktop files
install(FILES effects/logout/logout.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
