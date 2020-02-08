#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/magnifier/magnifier.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/magnifier/magnifierconfig.kcfgc)

# .desktop files
install(FILES effects/magnifier/magnifier.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/magnifier/magnifier_config.cpp
)
kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/magnifier/magnifierconfig.kcfgc)

# .desktop files
install(FILES effects/magnifier/magnifier_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
