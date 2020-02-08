
set( kcm_powerdevil_global_SRCS
     GeneralPage.cpp
     generalPage.ui
)

kde4_add_kcfg_files(kcm_powerdevil_global_SRCS ../../PowerDevilSettings.kcfgc)

kde4_add_plugin(kcm_powerdevilglobalconfig ${kcm_powerdevil_global_SRCS})

target_link_libraries(kcm_powerdevilglobalconfig
     ${KDE4_KDECORE_LIBS}
     ${KDE4_KDEUI_LIBRARY}
     ${KDE4_KIO_LIBS}
     ${KDE4_SOLID_LIBS}
     ${KDE4_KNOTIFYCONFIG_LIBRARY}
     powerdevilconfigcommonprivate
)

install(TARGETS kcm_powerdevilglobalconfig  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install( FILES powerdevilglobalconfig.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
