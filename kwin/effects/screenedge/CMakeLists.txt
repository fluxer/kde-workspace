#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/screenedge/screenedgeeffect.cpp
)

# .desktop files
install(FILES effects/screenedge/screenedgeeffect.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
