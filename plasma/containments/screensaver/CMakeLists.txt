set(desktop_SRCS
    desktop.cpp
    ../../toolboxes/internaltoolbox.cpp)

kde4_add_plugin(plasma_containment_saverdesktop ${desktop_SRCS})
target_link_libraries(plasma_containment_saverdesktop ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_containment_saverdesktop DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-saverdesktop.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

