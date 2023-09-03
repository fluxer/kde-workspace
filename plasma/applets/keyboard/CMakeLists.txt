project(plasma-applet-keyboard)

set(keyboard_SRCS
    keyboard.cpp
)

kde4_add_plugin(plasma_applet_keyboard ${keyboard_SRCS})
target_link_libraries(plasma_applet_keyboard
    KDE4::plasma
    KDE4::solid
)

install(
    TARGETS plasma_applet_keyboard
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-keyboard.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
