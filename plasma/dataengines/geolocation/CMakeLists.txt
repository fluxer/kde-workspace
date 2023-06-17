project(geolocation)

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_interface_SRCS geolocationprovider.cpp)
add_library(plasma-geolocation-interface SHARED ${plasma_geolocation_interface_SRCS})
target_link_libraries(plasma-geolocation-interface PUBLIC
    KDE4::kio
    KDE4::plasma
)

set_target_properties(plasma-geolocation-interface PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(plasma-geolocation-interface BASE_NAME geolocation)

install(
    TARGETS plasma-geolocation-interface
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

install(
    FILES
    geolocationprovider.h
    ${CMAKE_CURRENT_BINARY_DIR}/geolocation_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasma/geolocation
)

#install(FILES includes/Interface
#        DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/KDE/Plasma/Geolocation)
# -------------------------------------------------------------------------------------------------

set(plasma_dataengine_geolocation_SRCS geolocation.cpp)
kde4_add_plugin(plasma_engine_geolocation ${plasma_dataengine_geolocation_SRCS})
target_link_libraries(plasma_engine_geolocation
    plasma-geolocation-interface
    KDE4::plasma
    KDE4::kdecore
    KDE4::kio
    KDE4::solid
)

install(TARGETS plasma_engine_geolocation DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-geolocation.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(FILES plasma-geolocationprovider.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_hostip_SRCS location_hostip.cpp)
kde4_add_plugin(plasma-geolocation-hostip ${plasma_geolocation_hostip_SRCS})
target_link_libraries(plasma-geolocation-hostip plasma-geolocation-interface)
install(FILES plasma-geolocation-hostip.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS plasma-geolocation-hostip DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_ipinfo_SRCS location_ipinfo.cpp)
kde4_add_plugin(plasma-geolocation-ipinfo ${plasma_geolocation_ipinfo_SRCS})
target_link_libraries(plasma-geolocation-ipinfo plasma-geolocation-interface)
install(FILES plasma-geolocation-ipinfo.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS plasma-geolocation-ipinfo DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_geoplugin_SRCS location_geoplugin.cpp)
kde4_add_plugin(plasma-geolocation-geoplugin ${plasma_geolocation_geoplugin_SRCS})
target_link_libraries(plasma-geolocation-geoplugin plasma-geolocation-interface)
install(FILES plasma-geolocation-geoplugin.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS plasma-geolocation-geoplugin DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

# -------------------------------------------------------------------------------------------------

set(plasma_geolocation_mozilla_SRCS location_mozilla.cpp)
kde4_add_plugin(plasma-geolocation-mozilla ${plasma_geolocation_mozilla_SRCS})
target_link_libraries(plasma-geolocation-mozilla plasma-geolocation-interface)
install(FILES plasma-geolocation-mozilla.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS plasma-geolocation-mozilla DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
