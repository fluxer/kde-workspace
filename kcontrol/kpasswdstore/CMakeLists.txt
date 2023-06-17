########### next target ###############

set(kpasswdstoreconfig_SRCS 
    kpasswdstoreconfig.cpp
    kpasswdstoreconfig.ui
)

kde4_add_plugin(kcm_kpasswdstoreconfig ${kpasswdstoreconfig_SRCS})

target_link_libraries(kcm_kpasswdstoreconfig
    KDE4::kdeui
    KDE4::kcmutils
)

install(
    TARGETS kcm_kpasswdstoreconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_kpasswdstoreconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
