#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/presentwindows/presentwindows.cpp
    effects/presentwindows/presentwindows_proxy.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/presentwindows/presentwindowsconfig.kcfgc)

# .desktop files
install(FILES effects/presentwindows/presentwindows.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
install(FILES effects/presentwindows/main.qml DESTINATION ${DATA_INSTALL_DIR}/kwin/effects/presentwindows)

#######################################
# Config

# Source files
set(kwin4_effect_builtins_config_sources
    ${kwin4_effect_builtins_config_sources}
    effects/presentwindows/presentwindows_config.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_config_sources effects/presentwindows/presentwindowsconfig.kcfgc)

# .desktop files
install(FILES effects/presentwindows/presentwindows_config.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
