project(plasma-pager)

set(pager_SRCS
    model.cpp
    pager.cpp
    pagerConfig.ui
)

kde4_add_plugin(plasma_applet_pager ${pager_SRCS})
target_link_libraries(plasma_applet_pager
                      ${KDE4_KIO_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${KDE4_KCMUTILS_LIBS}
                      ${QT_QTDECLARATIVE_LIBRARY}
                      taskmanager)

install(TARGETS plasma_applet_pager DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-pager-default.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(DIRECTORY package/ DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/packages/org.kde.pager)
