


########### next target ###############

set(kcm_access_PART_SRCS kcmaccess.cpp )


kde4_add_plugin(kcm_access ${kcm_access_PART_SRCS})


target_link_libraries(kcm_access  ${KDE4_KIO_LIBS} ${KDE4_PHONON_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} ${X11_LIBRARIES})

install(TARGETS kcm_access  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(kaccess_SRCS kaccess.cpp main.cpp )


add_executable( kaccess ${kaccess_SRCS})

target_link_libraries(kaccess  ${KDE4_KDEUI_LIBS} ${KDE4_PHONON_LIBS} ${X11_LIBRARIES})

install(TARGETS kaccess         ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES kcmaccess.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kaccess.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kaccess.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kaccess/ )
