



########### next target ###############

set(kcm_kwallet_PART_SRCS konfigurator.cpp walletconfigwidget.ui)

kde4_add_plugin(kcm_kwallet ${kcm_kwallet_PART_SRCS})


target_link_libraries(kcm_kwallet ${KDE4_KDEUI_LIBS})

install(TARGETS kcm_kwallet  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kwalletconfig.desktop kwalletmanager_show.desktop  DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
