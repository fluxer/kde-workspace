# the Ion shared library
set(ionlib_SRCS ion.cpp)
add_library(weather_ion SHARED ${ionlib_SRCS})
target_link_libraries(weather_ion PUBLIC
    KDE4::kdeui
    KDE4::kio
    KDE4::plasma
)

set_target_properties(weather_ion PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(weather_ion BASE_NAME ion)

install(
    TARGETS weather_ion
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

install(
    FILES
    ion.h
    ${CMAKE_CURRENT_BINARY_DIR}/ion_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasma/weather
)

install(
    FILES includes/Ion
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/KDE/Plasma/Weather
)

# the individual ion plugins
add_subdirectory(noaa)
add_subdirectory(wetter.com)
