project(plasma-applet-sangria)

set(sangria_SRCS
    sangria.cpp)

kde4_add_plugin(plasma_applet_sangria ${sangria_SRCS})
target_link_libraries(plasma_applet_sangria ${KDE4_PLASMA_LIBS} ${KDE4_SOLID_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KCMUTILS_LIBS})

install(TARGETS plasma_applet_sangria DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-sangria.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
