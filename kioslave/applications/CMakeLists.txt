########### kio_applications ###############

kde4_add_plugin(kio_applications kio_applications.cpp)

target_link_libraries(kio_applications  ${KDE4_KIO_LIBS} )

install(TARGETS kio_applications  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  programs.protocol applications.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
