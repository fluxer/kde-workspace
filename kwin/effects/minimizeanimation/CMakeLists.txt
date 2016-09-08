#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/minimizeanimation/minimizeanimation.cpp
)

# .desktop files
install(FILES effects/minimizeanimation/minimizeanimation.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
