set(krunner_webshortcuts_SRCS
    webshortcutrunner.cpp
)

kde4_add_plugin(krunner_webshortcuts ${krunner_webshortcuts_SRCS})
target_link_libraries(krunner_webshortcuts ${KDE4_KIO_LIBS} ${KDE4_PLASMA_LIBS})

install(TARGETS krunner_webshortcuts DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-webshortcuts.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

