########### next target ###############

set(kcm_workspaceoptions_PART_SRCS workspaceoptions.cpp mainpage.ui)

kde4_add_plugin(kcm_workspaceoptions ${kcm_workspaceoptions_PART_SRCS})

target_link_libraries(kcm_workspaceoptions KDE4::kdeui KDE4::kio )

install(TARGETS kcm_workspaceoptions  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES workspaceoptions.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
