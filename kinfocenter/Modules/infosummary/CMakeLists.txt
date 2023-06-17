
set( infosummarySources infosum.cpp defaultboxwidget.cpp progressboxwidget.cpp osdepinfo.cpp)

kde4_add_plugin( kcm_infosummary ${infosummarySources} )
target_link_libraries( kcm_infosummary KDE4::kio KDE4::solid )

install( TARGETS kcm_infosummary DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install( FILES kcm_infosummary.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )
