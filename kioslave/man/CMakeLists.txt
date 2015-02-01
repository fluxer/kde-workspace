



########### next target ###############

set(kio_man_PART_SRCS man.cc )


kde4_add_plugin(kio_man ${kio_man_PART_SRCS})


target_link_libraries(kio_man  ${KDE4_KIO_LIBS} )

install(TARGETS kio_man  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES man.protocol  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( PROGRAMS kde-man2html DESTINATION  ${DATA_INSTALL_DIR}/kio_man )

