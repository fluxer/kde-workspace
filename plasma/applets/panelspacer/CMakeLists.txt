project(plasma-panelspacer-internal)

set(panelspacer_SRCS
    panelspacer.cpp)

kde4_add_plugin(plasma_applet_panelspacer_internal ${panelspacer_SRCS})
target_link_libraries(plasma_applet_panelspacer_internal KDE4::plasma)

install(TARGETS plasma_applet_panelspacer_internal DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})



install(FILES plasma-applet-panelspacer-internal.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

