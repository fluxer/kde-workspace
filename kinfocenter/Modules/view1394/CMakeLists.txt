########### next target ###############

set(kcm_view1394_PART_SRCS view1394.cpp view1394widget.ui)

kde4_add_plugin(kcm_view1394 ${kcm_view1394_PART_SRCS})

target_link_libraries(kcm_view1394 KDE4::kdeui ${RAW1394_LIBRARIES})

install(TARGETS kcm_view1394  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES kcmview1394.desktop DESTINATION  ${KDE4_SERVICES_INSTALL_DIR})
install(FILES oui.db DESTINATION ${KDE4_DATA_INSTALL_DIR}/kcmview1394)

