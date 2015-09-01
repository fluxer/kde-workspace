project (freespacenotifier)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

########### next target ###############

set(kded_freespacenotifier_SRCS freespacenotifier.cpp module.cpp freespacenotifier_prefs_base.ui)

kde4_add_kcfg_files(kded_freespacenotifier_SRCS settings.kcfgc)

kde4_add_plugin(kded_freespacenotifier ${kded_freespacenotifier_SRCS})

target_link_libraries(kded_freespacenotifier ${KDE4_KIO_LIBS} )

install(TARGETS kded_freespacenotifier  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES freespacenotifier.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )
install( FILES freespacenotifier.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/freespacenotifier )
install( FILES freespacenotifier.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
