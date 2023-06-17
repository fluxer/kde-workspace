include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/ions
)

ADD_SUBDIRECTORY(ions)

SET(weather_SRCS weatherengine.cpp)
kde4_add_plugin(plasma_engine_weather ${weather_SRCS})
TARGET_LINK_LIBRARIES(plasma_engine_weather
    KDE4::kio
    KDE4::solid
    KDE4::plasma
    weather_ion
)

INSTALL(
    TARGETS plasma_engine_weather
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
INSTALL(
    FILES plasma-dataengine-weather.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
