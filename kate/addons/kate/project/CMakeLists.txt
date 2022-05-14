set(kateprojectplugin_PART_SRCS
    kateprojectplugin.cpp
    kateprojectpluginview.cpp
    kateproject.cpp
    kateprojectworker.cpp
    kateprojectitem.cpp
    kateprojectview.cpp
    kateprojectviewtree.cpp
    kateprojecttreeviewcontextmenu.cpp
    kateprojectinfoview.cpp
    kateprojectcompletion.cpp
    kateprojectindex.cpp
    kateprojectinfoviewindex.cpp
    kateprojectinfoviewcodeanalysis.cpp
    kateprojectinfoviewnotes.cpp
    kateprojectnew.cpp
)

kde4_add_plugin(kateprojectplugin ${kateprojectplugin_PART_SRCS})

target_link_libraries(kateprojectplugin
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    kateinterfaces
)

########### install files ###############
install(
    TARGETS kateprojectplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES
    ui.rc
    kateproject.example
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/project
)
install(
    FILES kateprojectplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
