set(krunner_services_SRCS
    servicerunner.cpp
)

kde4_add_plugin(krunner_services ${krunner_services_SRCS})
target_link_libraries(krunner_services ${KDE4_KIO_LIBS} ${KDE4_PLASMA_LIBS})

install(TARGETS krunner_services DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES plasma-runner-services.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

