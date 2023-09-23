project(plasmacomponents)

set(plasmacomponents_SRCS
    plasmacomponentsplugin.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

add_library(plasmacomponentsplugin SHARED ${plasmacomponents_SRCS})
target_link_libraries(plasmacomponentsplugin
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    KDE4::kdeui
    KDE4::plasma
)

install(
    TARGETS plasmacomponentsplugin
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/components
)

install(
    DIRECTORY qml/
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/components
)

