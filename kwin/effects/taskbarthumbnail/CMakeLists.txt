#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/taskbarthumbnail/taskbarthumbnail.cpp
)

# .desktop files
install(FILES effects/taskbarthumbnail/taskbarthumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin)