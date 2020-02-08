#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
     effects/dashboard/dashboard.cpp
     effects/dashboard/dashboard.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/dashboard/dashboardconfig.kcfgc)

# .desktop files
install(FILES effects/dashboard/dashboard.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/dashboard/dashboard_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/dashboard/dashboardconfig.kcfgc)

install(FILES effects/dashboard/dashboard_config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
