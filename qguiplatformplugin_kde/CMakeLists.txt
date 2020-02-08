project(qguiplatformplugin_kde)

set(kde_SOURCES  qguiplatformplugin_kde.cpp)

kde4_add_plugin(libkde ${kde_SOURCES})

target_link_libraries(libkde ${KDE4_KIO_LIBS} ${QT_QTGUI_LIBRARY})

install(
    TARGETS libkde LIBRARY
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}/kde4/plugins/gui_platform
)




