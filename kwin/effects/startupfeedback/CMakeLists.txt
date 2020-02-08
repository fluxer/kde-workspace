#######################################
# Effect

# Source files
set( kwin4_effect_builtins_sources ${kwin4_effect_builtins_sources}
    effects/startupfeedback/startupfeedback.cpp
    )

# .desktop files
install( FILES
    effects/startupfeedback/startupfeedback.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin )

#######################################
# Config
