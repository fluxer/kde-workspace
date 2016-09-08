#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/resize/resize.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/resize/resizeconfig.kcfgc)

# .desktop files
install(FILES effects/resize/resize.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/resize/resize_config.cpp
)
kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/resize/resizeconfig.kcfgc)

# .desktop files
install(FILES effects/resize/resize_config.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
