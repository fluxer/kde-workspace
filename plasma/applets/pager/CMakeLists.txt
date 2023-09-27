project(plasma-applet-pager)

set(pager_SRCS
    pager.cpp
)

kde4_add_plugin(plasma_applet_pager ${pager_SRCS})
target_link_libraries(plasma_applet_pager
    KDE4::plasma
    KDE4::kcmutils
)

install(
    TARGETS plasma_applet_pager
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-pager.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
