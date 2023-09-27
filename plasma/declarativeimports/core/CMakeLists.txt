project(corebindings)

set(corebindings_SRCS
    declarativeitemcontainer.cpp
    corebindingsplugin.cpp
    theme.cpp
    svgitem.cpp
    framesvgitem.cpp
    dialog.cpp
    tooltip.cpp
    iconitem.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${KDECLARATIVE_INCLUDE_DIR}
)

add_library(corebindingsplugin SHARED ${corebindings_SRCS})
target_link_libraries(corebindingsplugin
    KDE4::plasma
    KDE4::kdeclarative
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
)

install(
    TARGETS corebindingsplugin
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/core
)

install(
    FILES qmldir
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/core
)
