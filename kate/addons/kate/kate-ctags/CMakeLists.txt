include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ctagsplugin_SRC
    readtags.c
    tags.cpp
    ctagskinds.cpp
    kate_ctags_view.cpp
    kate_ctags_plugin.cpp
)

kde4_add_plugin(katectagsplugin ${ctagsplugin_SRC})

target_link_libraries(katectagsplugin
    KDE4::kdecore
    kateinterfaces
    KDE4::ktexteditor
)

install(
    TARGETS katectagsplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katectags
)
install(
    FILES katectagsplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
