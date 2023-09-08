project(plasmaextracomponents)

set(plasmaextracomponents_SRCS
    plasmaextracomponentsplugin.cpp
    fallbackcomponent.cpp
)

add_library(plasmaextracomponentsplugin SHARED ${plasmaextracomponents_SRCS})
target_link_libraries(plasmaextracomponentsplugin
    KDE4::plasma
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
)

install(
    TARGETS plasmaextracomponentsplugin
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/extras
)

install(
    DIRECTORY qml/
    DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/extras
)
