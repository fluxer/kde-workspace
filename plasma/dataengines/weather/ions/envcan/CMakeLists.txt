SET (ion_envcan_SRCS ion_envcan.cpp)
kde4_add_plugin(ion_envcan ${ion_envcan_SRCS})
target_link_libraries (ion_envcan ${KDE4_SOLID_LIBS} weather_ion)

INSTALL (FILES ion-envcan.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

INSTALL (TARGETS ion_envcan DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

