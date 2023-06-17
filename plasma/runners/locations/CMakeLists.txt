set(krunner_locations_SRCS
    locationrunner.cpp
)

kde4_add_plugin(krunner_locations ${krunner_locations_SRCS})
target_link_libraries(krunner_locations KDE4::kio KDE4::plasma)

install(TARGETS krunner_locations DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-locations.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

