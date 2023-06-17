SET( icon_mode_srcs
     IconMode.cpp
     CategoryDrawer.cpp
     CategorizedView.cpp
) 

KDE4_ADD_PLUGIN(icon_mode ${icon_mode_srcs})

TARGET_LINK_LIBRARIES(icon_mode KDE4::kio KDE4::kcmutils systemsettingsview )

INSTALL( TARGETS icon_mode DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
INSTALL( FILES settings-icon-view.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )

