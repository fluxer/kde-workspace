project(plasma-applet-calculator)

set(calculator_SRCS
    calculator.cpp
)

kde4_add_plugin(plasma_applet_calculator ${calculator_SRCS})
target_link_libraries(plasma_applet_calculator
    KDE4::plasma
)

install(
    TARGETS plasma_applet_calculator
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-calculator.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
