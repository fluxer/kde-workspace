########### next target ###############

set(katetabbarextensionplugin_PART_SRCS
    plugin_katetabbarextension.cpp
    ktinytabbutton.cpp
    ktinytabbar.cpp
    ktinytabbarconfigdialog.cpp
    ktinytabbarconfigpage.cpp
)

kde4_add_plugin(katetabbarextensionplugin ${katetabbarextensionplugin_PART_SRCS})

target_link_libraries(katetabbarextensionplugin
    KDE4::kdeui
    KDE4::kparts
    kateinterfaces
)

install(
    TARGETS katetabbarextensionplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katetabbarextension
)
install(
    FILES katetabbarextension.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
