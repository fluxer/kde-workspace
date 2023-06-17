set(ion_wettercom_SRCS ion_wettercom.cpp)
kde4_add_plugin(ion_wettercom ${ion_wettercom_SRCS})
target_link_libraries(ion_wettercom weather_ion KDE4::kdecore KDE4::plasma KDE4::kio)

install(FILES ion-wettercom.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(TARGETS ion_wettercom DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

