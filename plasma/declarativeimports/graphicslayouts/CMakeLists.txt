project(graphicslayoutsbindings)

set(graphicslayoutsbindings_SRCS
    graphicslayoutsbindingsplugin.cpp
    gridlayout.cpp
    linearlayout.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

add_library(graphicslayoutsbindingsplugin SHARED ${graphicslayoutsbindings_SRCS})
target_link_libraries(graphicslayoutsbindingsplugin ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSCRIPT_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS graphicslayoutsbindingsplugin DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasma/graphicslayouts)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/plasma/graphicslayouts)
