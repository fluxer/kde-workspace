project(plasma-applet-calendar)

include_directories(
    # for plasmaclock_export.h
    ${CMAKE_BINARY_DIR}/libs/plasmaclock
)

set(calendar_SRCS
    calendar.cpp
)

kde4_add_plugin(plasma_applet_calendar ${calendar_SRCS})
target_link_libraries(plasma_applet_calendar
    KDE4::plasma
    KDE4::kio
    plasmaclock
)

install(
    TARGETS plasma_applet_calendar
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-applet-calendar.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES mini-calendar.svgz
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/default/calendar/
)
