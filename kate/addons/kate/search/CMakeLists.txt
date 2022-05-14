set(katesearchplugin_PART_SRCS
    plugin_search.cpp
    search_open_files.cpp
    SearchDiskFiles.cpp
    FolderFilesList.cpp
    replace_matches.cpp
    htmldelegate.cpp
)

kde4_add_plugin(katesearchplugin ${katesearchplugin_PART_SRCS})

target_link_libraries(katesearchplugin
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    kateinterfaces
)

########### install files ###############

install(
    TARGETS katesearchplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katesearch
)
install(
    FILES katesearch.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

