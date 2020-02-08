#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/snaphelper/snaphelper.cpp
)

# .desktop files
install(FILES  effects/snaphelper/snaphelper.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
