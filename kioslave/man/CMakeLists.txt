



########### next target ###############

set(kio_man_PART_SRCS man.cc )


kde4_add_plugin(kio_man ${kio_man_PART_SRCS})


target_link_libraries(kio_man  ${KDE4_KIO_LIBS} )

install(TARGETS kio_man  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES man.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( PROGRAMS kde-man2html DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kio_man )

