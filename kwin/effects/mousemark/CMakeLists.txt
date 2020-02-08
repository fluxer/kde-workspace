#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/mousemark/mousemark.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/mousemark/mousemarkconfig.kcfgc)

# .desktop files
install(FILES effects/mousemark/mousemark.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/mousemark/mousemark_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/mousemark/mousemarkconfig.kcfgc)

# .desktop files
install(FILES effects/mousemark/mousemark_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
