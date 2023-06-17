SET (ion_noaa_SRCS ion_noaa.cpp ../dataengineconsumer.cpp)
kde4_add_plugin(ion_noaa ${ion_noaa_SRCS})
target_link_libraries (ion_noaa KDE4::solid weather_ion)

INSTALL (FILES ion-noaa.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

INSTALL (TARGETS ion_noaa DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

