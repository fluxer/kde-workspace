#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
     effects/kscreen/kscreen.cpp
)

kde4_add_kcfg_files(kwin4_effect_builtins_sources effects/kscreen/kscreenconfig.kcfgc)

# .desktop files
install(FILES effects/kscreen/kscreen.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
