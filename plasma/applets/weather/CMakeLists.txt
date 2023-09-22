project(plasma-applet-weather)

set(weather_SRCS
    weather.cpp
)

kde4_add_plugin(plasma_applet_weather ${weather_SRCS})
target_link_libraries(plasma_applet_weather
    KDE4::plasma
    KDE4::kio
)

install(
    TARGETS plasma_applet_weather
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-weather.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
