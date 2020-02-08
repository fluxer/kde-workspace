#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/showfps/showfps.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/showfps/showfpsconfig.kcfgc)

# .desktop files
install(FILES effects/showfps/showfps.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/showfps/showfps_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/showfps/showfpsconfig.kcfgc)

# .desktop files
install(FILES effects/showfps/showfps_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
