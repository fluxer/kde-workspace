project(plasma-containmentactions-minimalcontextmenu)

set(contextmenu_SRCS
    menu.cpp
)

kde4_add_plugin(plasma_containmentactions_minimalcontextmenu ${contextmenu_SRCS})
target_link_libraries(plasma_containmentactions_minimalcontextmenu ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_containmentactions_minimalcontextmenu DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containmentactions-minimalcontextmenu.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
