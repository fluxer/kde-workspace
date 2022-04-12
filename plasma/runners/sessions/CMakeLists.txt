set(krunner_sessions_SRCS
    sessionrunner.cpp
)

set(screensaver_xml ${CMAKE_SOURCE_DIR}/kscreensaver/org.freedesktop.ScreenSaver.xml)
QT4_ADD_DBUS_INTERFACE(krunner_sessions_SRCS ${screensaver_xml} screensaver_interface)

kde4_add_plugin(krunner_sessions ${krunner_sessions_SRCS})
target_link_libraries(krunner_sessions ${KDE4_KIO_LIBS} ${KDE4_PLASMA_LIBS} kworkspace)

install(TARGETS krunner_sessions DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-sessions.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

