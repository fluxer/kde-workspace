set(plastik_plugin_SRCS
    plastikbutton.cpp
    plastikplugin.cpp
)

add_library(plastikplugin SHARED ${plastik_plugin_SRCS})
target_link_libraries(plastikplugin ${QT_QTCORE_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} KDE4::kdeui kdecorations)
install(TARGETS plastikplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/kwin/decorations/plastik)
install(FILES qmldir DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/kwin/decorations/plastik)
