set(fixhosturifilter_PART_SRCS fixhosturifilter.cpp )

kde4_add_plugin(fixhosturifilter ${fixhosturifilter_PART_SRCS})

target_link_libraries(fixhosturifilter KDE4::kio)

install(TARGETS fixhosturifilter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES fixhosturifilter.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

