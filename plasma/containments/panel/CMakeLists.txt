
set(panel_SRCS
    panel.cpp
)

kde4_add_plugin(plasma_containment_panel ${panel_SRCS})
target_link_libraries(plasma_containment_panel KDE4::plasma KDE4::kio)

install(TARGETS plasma_containment_panel DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-panel.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
