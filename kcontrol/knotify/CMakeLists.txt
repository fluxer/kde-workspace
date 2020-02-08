
########### next target ###############

set(kcm_knotify_PART_SRCS knotify.cpp playersettings.ui )

kde4_add_plugin(kcm_knotify ${kcm_knotify_PART_SRCS})

target_link_libraries(kcm_knotify ${KDE4_KIO_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} ${QT_QTGUI_LIBRARY})

install(TARGETS kcm_knotify  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES kcmnotify.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})


