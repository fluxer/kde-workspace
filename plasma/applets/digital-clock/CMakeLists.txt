project(plasma-dig-clock)

include_directories(
    # for plasmaclock_export.h
    ${CMAKE_BINARY_DIR}/libs/plasmaclock
)

set(clock_SRCS
    clock.cpp
    clockConfig.ui
)

kde4_add_plugin(plasma_applet_dig_clock ${clock_SRCS})
target_link_libraries(plasma_applet_dig_clock
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    plasmaclock
)

install(TARGETS plasma_applet_dig_clock DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-digitalclock.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

