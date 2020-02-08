



########### next target ###############

set(renaudioplugin_PART_SRCS audiopreview.cpp audio_plugin.cpp )

kde4_add_plugin(renaudioplugin WITH_PREFIX ${renaudioplugin_PART_SRCS})



target_link_libraries(renaudioplugin  ${KDE4_KIO_LIBS} kmediaplayer )

install(TARGETS renaudioplugin  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES renaudiodlg.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

