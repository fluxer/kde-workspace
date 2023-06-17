project(localebindings)

set(localebindings_SRCS
    locale.cpp
    localebindingsplugin.cpp
    calendarsystem.cpp
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

add_library(localebindingsplugin SHARED ${localebindings_SRCS})

target_link_libraries(localebindingsplugin ${QT_QTDECLARATIVE_LIBRARY} KDE4::kdecore)

install(TARGETS localebindingsplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/locale)
install(FILES qmldir DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/locale)
