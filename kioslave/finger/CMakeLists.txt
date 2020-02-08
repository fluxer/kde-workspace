



########### next target ###############

set(kio_finger_PART_SRCS kio_finger.cpp )


kde4_add_plugin(kio_finger ${kio_finger_PART_SRCS})


target_link_libraries(kio_finger ${KDE4_KIO_LIBS} )

install(TARGETS kio_finger  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kio_finger.pl kio_finger.css  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kio_finger )
install( FILES finger.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )


