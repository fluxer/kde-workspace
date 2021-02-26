
set( kcm_powerdevil_profiles_SRCS
     EditPage.cpp
     ../../daemon/powerdevilprofilegenerator.cpp
    profileEditPage.ui
)

kde4_add_kcfg_files(kcm_powerdevil_profiles_SRCS ../../PowerDevilSettings.kcfgc)

kde4_add_plugin(kcm_powerdevilprofilesconfig ${kcm_powerdevil_profiles_SRCS})

target_link_libraries(kcm_powerdevilprofilesconfig
     ${KDE4_KDECORE_LIBS}
     ${KDE4_KDEUI_LIBS}
     ${KDE4_KIO_LIBS}
     ${KDE4_SOLID_LIBS}
     powerdevilui
     powerdevilconfigcommonprivate
)

install(TARGETS kcm_powerdevilprofilesconfig  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install( FILES powerdevilprofilesconfig.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
