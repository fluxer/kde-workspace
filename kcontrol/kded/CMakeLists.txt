


########### next target ###############

set(kcm_kded_PART_SRCS kcmkded.cpp )


kde4_add_plugin(kcm_kded ${kcm_kded_PART_SRCS})


target_link_libraries(kcm_kded KDE4::kdeui)

install(TARGETS kcm_kded  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcmkded.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
