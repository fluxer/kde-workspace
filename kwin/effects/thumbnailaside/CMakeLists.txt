#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/thumbnailaside/thumbnailaside.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/thumbnailaside/thumbnailasideconfig.kcfgc)

# .desktop files
install(FILES effects/thumbnailaside/thumbnailaside.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/thumbnailaside/thumbnailaside_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/thumbnailaside/thumbnailasideconfig.kcfgc)

# .desktop files
install(FILES effects/thumbnailaside/thumbnailaside_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
