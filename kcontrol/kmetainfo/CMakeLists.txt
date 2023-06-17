########### next target ###############

set(kmetainfoconfig_SRCS 
    kmetainfoconfig.cpp
    kmetainfoconfig.ui
)

kde4_add_plugin(kcm_kmetainfoconfig ${kmetainfoconfig_SRCS})

target_link_libraries(kcm_kmetainfoconfig
    KDE4::kdeui
    KDE4::kcmutils
    KDE4::kio
)

install(
    TARGETS kcm_kmetainfoconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_kmetainfoconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
