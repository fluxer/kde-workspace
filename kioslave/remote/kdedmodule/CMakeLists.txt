


########### next target ###############

set(kded_remotedirnotify_PART_SRCS remotedirnotify.cpp remotedirnotifymodule.cpp )


kde4_add_plugin(kded_remotedirnotify ${kded_remotedirnotify_PART_SRCS})


target_link_libraries(kded_remotedirnotify  KDE4::kio)

install(TARGETS kded_remotedirnotify  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES remotedirnotify.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/kded )

