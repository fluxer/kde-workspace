set(places_engine_SRCS
    placesengine.cpp
    placeservice.cpp
    setupdevicejob.cpp
)

kde4_add_plugin(plasma_engine_places ${places_engine_SRCS})

target_link_libraries(plasma_engine_places
    plasma
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(TARGETS plasma_engine_places
        DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-places.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )
install(FILES org.kde.places.operations
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/services )

