set(krunner_plasma-desktop_SRCS
    plasma-desktop-runner.cpp
)

kde4_add_plugin(krunner_plasma-desktop ${krunner_plasma-desktop_SRCS})
target_link_libraries(krunner_plasma-desktop KDE4::kio KDE4::plasma)

install(TARGETS krunner_plasma-desktop DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-plasma-desktop.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

