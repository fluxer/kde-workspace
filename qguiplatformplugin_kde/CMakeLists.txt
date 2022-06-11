project(qguiplatformplugin_kde)

set(kde_SOURCES  qguiplatformplugin_kde.cpp)

kde4_add_plugin(kde ${kde_SOURCES})

target_link_libraries(kde ${KDE4_KIO_LIBS} ${QT_QTGUI_LIBRARY})

install(
    TARGETS kde LIBRARY
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}/kde4/plugins/gui_platform
)




