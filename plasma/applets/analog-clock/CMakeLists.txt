
set(clock_SRCS
    clock.cpp
    clockConfig.ui
)

kde4_add_plugin(plasma_applet_clock ${clock_SRCS})
target_link_libraries(plasma_applet_clock ${KDE4_PLASMA_LIBS} plasmaclock ${KDE4_KIO_LIBS})

install(TARGETS plasma_applet_clock DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-analogclock.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
