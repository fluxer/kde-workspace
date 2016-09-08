#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/windowgeometry/windowgeometry.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/windowgeometry/windowgeometryconfig.kcfgc)

# .desktop files
install(FILES effects/windowgeometry/windowgeometry.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/windowgeometry/windowgeometry_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/windowgeometry/windowgeometryconfig.kcfgc)

# .desktop files
install(FILES effects/windowgeometry/windowgeometry_config.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
