project(graphicswidgetsbindings)

set(graphicswidgetsbindings_SRCS
    graphicswidgetsbindingsplugin.cpp
    declarativetabbar.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

add_library(graphicswidgetsbindingsplugin SHARED ${graphicswidgetsbindings_SRCS})
target_link_libraries(graphicswidgetsbindingsplugin
    ${KDE4_PLASMA_LIBS}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
)

install(
    TARGETS graphicswidgetsbindingsplugin
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/graphicswidgets
)
install(
    FILES qmldir
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/graphicswidgets
)
