#######################################
# Effect

# Source files
set( kwin4_effect_builtins_sources ${kwin4_effect_builtins_sources}
    logout/logout.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources logout/logoutconfig.kcfgc)

# .desktop files
install( FILES
    logout/logout.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}/kwin
)
