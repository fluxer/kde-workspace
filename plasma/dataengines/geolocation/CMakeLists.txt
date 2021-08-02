project(geolocation)

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_interface_SRCS geolocationprovider.cpp)
add_library(plasma-geolocation-interface SHARED ${plasma_geolocation_interface_SRCS})
target_link_libraries(plasma-geolocation-interface PUBLIC
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
)

set_target_properties(plasma-geolocation-interface PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(plasma-geolocation-interface BASE_NAME geolocation)

install(TARGETS plasma-geolocation-interface ${INSTALL_TARGETS_DEFAULT_ARGS})

install(
    FILES
    geolocationprovider.h
    ${CMAKE_CURRENT_BINARY_DIR}/geolocation_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasma/geolocation
    COMPONENT Devel
)

#install(FILES includes/Interface
#        DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/KDE/Plasma/Geolocation
#        COMPONENT Devel)
# -------------------------------------------------------------------------------------------------

set(plasma_dataengine_geolocation_SRCS geolocation.cpp)
kde4_add_plugin(plasma_engine_geolocation ${plasma_dataengine_geolocation_SRCS})
target_link_libraries(plasma_engine_geolocation
    plasma-geolocation-interface
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(TARGETS plasma_engine_geolocation DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-geolocation.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(FILES plasma-geolocationprovider.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_ip_SRCS location_ip.cpp)
kde4_add_plugin(plasma-geolocation-ip ${plasma_geolocation_ip_SRCS})
target_link_libraries(plasma-geolocation-ip plasma-geolocation-interface)
install(FILES plasma-geolocation-ip.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS plasma-geolocation-ip DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------
