
set(kio_recentdocuments_PART_SRCS
   recentdocuments.cpp)

kde4_add_plugin(kio_recentdocuments ${kio_recentdocuments_PART_SRCS})

target_link_libraries(kio_recentdocuments ${KDE4_KIO_LIBS})

install(TARGETS kio_recentdocuments  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(kded_recentdocumentsnotifier_PART_SRCS  recentdocumentsnotifier.cpp )

kde4_add_plugin(kded_recentdocumentsnotifier  ${kded_recentdocumentsnotifier_PART_SRCS})

target_link_libraries(kded_recentdocumentsnotifier  ${KDE4_KIO_LIBS})

install(TARGETS kded_recentdocumentsnotifier  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES recentdocumentsnotifier.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/kded )
install( FILES recentdocuments.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
