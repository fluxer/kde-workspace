########### next target ###############

set(kdebugconfig_SRCS 
    kdebugconfig.cpp
    kdebugconfig.ui
)

kde4_add_plugin(kcm_kdebugconfig ${kdebugconfig_SRCS})

target_link_libraries(kcm_kdebugconfig ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS})

install(TARGETS kcm_kdebugconfig DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES kcm_kdebugconfig.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
