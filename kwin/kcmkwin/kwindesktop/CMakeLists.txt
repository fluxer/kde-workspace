
########### next target ###############

set(kcm_kwindesktop_PART_SRCS main.cpp desktopnameswidget.cpp main.ui)

kde4_add_plugin(kcm_kwindesktop ${kcm_kwindesktop_PART_SRCS})

target_link_libraries(kcm_kwindesktop  KDE4::kdeui KDE4::kcmutils ${X11_LIBRARIES})

install(TARGETS kcm_kwindesktop  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############
install( FILES desktop.desktop DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

