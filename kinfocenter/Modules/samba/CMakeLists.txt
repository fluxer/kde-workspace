set(kcm_samba_PART_SRCS 
   main.cpp 
   ksmbstatus.cpp 
   kcmsambaimports.cpp 
   kcmsambalog.cpp 
   kcmsambastatistics.cpp )


kde4_add_plugin(kcm_samba ${kcm_samba_PART_SRCS})


target_link_libraries(kcm_samba ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kcm_samba  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES smbstatus.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
