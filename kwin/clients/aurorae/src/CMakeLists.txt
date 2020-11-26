########### decoration ###############
include_directories(
        ./lib
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(kwin3_aurorae_PART_SRCS
    aurorae.cpp
    lib/auroraetheme.cpp
    lib/themeconfig.cpp
)

kde4_add_plugin(kwin3_aurorae ${kwin3_aurorae_PART_SRCS})

target_link_libraries(kwin3_aurorae ${KDE4_KDEUI_LIBS} ${QT_QTDECLARATIVE_LIBRARY} kdecorations)

install(TARGETS kwin3_aurorae  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

set(decoration_plugin_SRCS
    decorationplugin.cpp
    decorationoptions.cpp
    colorhelper.cpp
)

add_library(decorationplugin SHARED ${decoration_plugin_SRCS})
target_link_libraries(decorationplugin ${QT_QTCORE_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${KDE4_KDEUI_LIBRARY} kdecorations)
install(TARGETS decorationplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/kwin/decoration)

########### install files ###############

install( FILES aurorae.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kwin )
install( FILES
    qml/aurorae.qml
    qml/AuroraeButton.qml
    qml/AuroraeButtonGroup.qml
    qml/AuroraeMaximizeButton.qml
    qml/Decoration.qml
    qml/DecorationButton.qml
    qml/MenuButton.qml
    qml/AppMenuButton.qml
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/aurorae )
install( FILES
    qml/Decoration.qml
    qml/DecorationButton.qml
    qml/MenuButton.qml
    qml/AppMenuButton.qml
    qml/ButtonGroup.qml
    qml/qmldir
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/kwin/decoration )
install( FILES kwindecoration.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR} )
