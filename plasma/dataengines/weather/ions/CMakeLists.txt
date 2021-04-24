# the Ion shared library
set(ionlib_SRCS ion.cpp)
add_library (weather_ion SHARED ${ionlib_SRCS})
target_link_libraries(weather_ion PUBLIC
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
)

set_target_properties(weather_ion PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS weather_ion
    EXPORT kdeworkspaceLibraryTargets
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install(
    FILES ion.h ion_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasma/weather
    COMPONENT Devel
)

install(
    FILES includes/Ion
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/KDE/Plasma/Weather
    COMPONENT Devel
)

# the individual ion plugins
add_subdirectory(noaa)
add_subdirectory(wetter.com)
