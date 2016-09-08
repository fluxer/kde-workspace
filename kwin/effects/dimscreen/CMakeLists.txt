#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/dimscreen/dimscreen.cpp
)

# .desktop files
install(FILES effects/dimscreen/dimscreen.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)
