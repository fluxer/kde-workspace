set(krunner_windows_SRCS
    windowsrunner.cpp
)

kde4_add_plugin(krunner_windows ${krunner_windows_SRCS})
target_link_libraries(krunner_windows KDE4::kdeui KDE4::plasma)

install(TARGETS krunner_windows DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-windows.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
