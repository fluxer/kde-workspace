


########### next target ###############

set(kshorturifilter_PART_SRCS kshorturifilter.cpp )


kde4_add_plugin(kshorturifilter ${kshorturifilter_PART_SRCS})


target_link_libraries(kshorturifilter  ${KDE4_KIO_LIBS})

install(TARGETS kshorturifilter  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kshorturifilter.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES kshorturifilterrc  DESTINATION  ${KDE4_CONFIG_INSTALL_DIR} )

