
set(panel_SRCS
    panel.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/kephal)

kde4_add_plugin(plasma_containment_panel ${panel_SRCS})
target_link_libraries(plasma_containment_panel ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} kephal)

install(TARGETS plasma_containment_panel DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-panel.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
