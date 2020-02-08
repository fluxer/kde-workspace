set(localdomainurifilter_PART_SRCS localdomainurifilter.cpp )

kde4_add_plugin(localdomainurifilter ${localdomainurifilter_PART_SRCS})

target_link_libraries(localdomainurifilter ${KDE4_KIO_LIBS})

install(TARGETS localdomainurifilter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES localdomainurifilter.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
