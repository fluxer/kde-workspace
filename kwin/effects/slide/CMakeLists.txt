#######################################
# Effect

# Source files
set(kwin4_effect_builtins_sources
    ${kwin4_effect_builtins_sources}
    effects/slide/slide.cpp
)

# .desktop files
install(FILES effects/slide/slide.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin)
