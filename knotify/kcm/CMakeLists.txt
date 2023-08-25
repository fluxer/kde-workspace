########### next target ###############

set(knotificationconfig_SRCS 
    knotificationconfig.cpp
)

kde4_add_plugin(kcm_knotificationconfig ${knotificationconfig_SRCS})

target_link_libraries(kcm_knotificationconfig
    KDE4::kdeui
    KDE4::kcmutils
)

install(
    TARGETS kcm_knotificationconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_knotificationconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
