


########### next target ###############

set(kcm_smartcard_PART_SRCS smartcard.cpp smartcardbase.ui nosmartcardbase.ui )

kde4_add_plugin(kcm_smartcard ${kcm_smartcard_PART_SRCS})

target_link_libraries(kcm_smartcard   ksmartcard ${KDE4_KIO_LIBS} )

install(TARGETS kcm_smartcard  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES smartcard.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )




