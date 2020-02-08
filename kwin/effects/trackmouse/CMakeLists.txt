#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/trackmouse/trackmouse.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/trackmouse/trackmouseconfig.kcfgc)

# .desktop files
install(FILES effects/trackmouse/trackmouse.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

# Data files
install(FILES
    effects/trackmouse/data/tm_inner.png
    effects/trackmouse/data/tm_outer.png
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin
)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/trackmouse/trackmouse_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/trackmouse/trackmouseconfig.kcfgc)

# .desktop files
install(FILES effects/trackmouse/trackmouse_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
