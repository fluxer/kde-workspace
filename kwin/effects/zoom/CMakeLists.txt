#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/zoom/zoom.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/zoom/zoomconfig.kcfgc)

# .desktop files
install(FILES effects/zoom/zoom.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/zoom/zoom_config.cpp
)
kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/zoom/zoomconfig.kcfgc)

# .desktop files
install(FILES effects/zoom/zoom_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
