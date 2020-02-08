#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/slideback/slideback.cpp
)

# .desktop files
install(FILES effects/slideback/slideback.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
