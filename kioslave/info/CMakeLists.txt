



########### next target ###############

set(kio_info_PART_SRCS info.cc )


kde4_add_plugin(kio_info ${kio_info_PART_SRCS})


target_link_libraries(kio_info  ${KDE4_KIO_LIBS} )

install(TARGETS kio_info  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kde-info2html.conf DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kio_info )
install( FILES info.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( PROGRAMS kde-info2html DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kio_info )

