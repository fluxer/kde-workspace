########### next target ###############

set(kdirshareplugin_PART_SRCS
    kdirshareplugin.cpp
    kdirshareplugin.ui
)

kde4_add_plugin(kdirshareplugin ${kdirshareplugin_PART_SRCS})
target_link_libraries(kdirshareplugin
    KDE4::kio
    KDE4::kio
    ${QT_QTDBUS_LIBRARY}
)

install(
    TARGETS kdirshareplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kdirshareplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
