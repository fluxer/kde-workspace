project(ktexteditor_exporter)

########### next target ###############

set(ktexteditor_exporter_PART_SRCS
    exporterplugin.cpp
    exporterpluginview.cpp
    htmlexporter.cpp
)

kde4_add_plugin(ktexteditor_exporter ${ktexteditor_exporter_PART_SRCS})

target_link_libraries(ktexteditor_exporter
    KDE4::kdecore
    KDE4::ktexteditor
)

install(
    TARGETS ktexteditor_exporter
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ktexteditor_exporter.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES ktexteditor_exporterui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/ktexteditor_exporter
)
