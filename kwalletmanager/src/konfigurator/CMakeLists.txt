



########### next target ###############

set(kcm_kwallet_PART_SRCS konfigurator.cpp walletconfigwidget.ui)

kde4_add_plugin(kcm_kwallet ${kcm_kwallet_PART_SRCS})


target_link_libraries(kcm_kwallet ${KDE4_KDEUI_LIBS})

install(TARGETS kcm_kwallet  DESTINATION ${PLUGIN_INSTALL_DIR})

########### kauth helper ################
set(kcm_kwallet_helper_SRCS savehelper.cpp)

add_executable(kcm_kwallet_helper ${kcm_kwallet_helper_SRCS})
target_link_libraries(kcm_kwallet_helper ${KDE4_KDECORE_LIBS})

install(TARGETS kcm_kwallet_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_helper_files(kcm_kwallet_helper org.kde.kcontrol.kcmkwallet root)

########### install files ###############

install( FILES kwalletconfig.desktop kwalletmanager_show.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
