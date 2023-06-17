set(krunner_webshortcuts_SRCS
    webshortcutrunner.cpp
)

kde4_add_plugin(krunner_webshortcuts ${krunner_webshortcuts_SRCS})
target_link_libraries(krunner_webshortcuts KDE4::kio KDE4::plasma)

install(TARGETS krunner_webshortcuts DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-webshortcuts.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

