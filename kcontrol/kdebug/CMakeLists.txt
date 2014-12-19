########### next target ###############

set(kdebugconfig_SRCS 
   kdebugconfig.cpp)

kde4_add_ui_files(kdebugconfig_SRCS kdebugconfig.ui)

kde4_add_plugin(kcm_kdebugconfig ${kdebugconfig_SRCS})

target_link_libraries(kcm_kdebugconfig  ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBRARY})

install(TARGETS kcm_kdebugconfig DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kdebugconfig.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
