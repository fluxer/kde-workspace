########### next target ###############

set(kfreespaceconfig_SRCS 
    kfreespaceconfig.cpp
)

kde4_add_plugin(kcm_kfreespaceconfig ${kfreespaceconfig_SRCS})

target_link_libraries(kcm_kfreespaceconfig
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(
    TARGETS kcm_kfreespaceconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_kfreespaceconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
