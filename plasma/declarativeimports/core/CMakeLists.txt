project(corebindings)

set(corebindings_SRCS
    declarativeitemcontainer.cpp
    corebindingsplugin.cpp
    dataengineconsumer.cpp
    theme.cpp
    datamodel.cpp
    datasource.cpp
    runnermodel.cpp
    svgitem.cpp
    framesvgitem.cpp
    dialog.cpp
    tooltip.cpp
    dataenginebindings.cpp
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

install(TARGETS corebindingsplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/core)
install(FILES qmldir DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/plasma/core)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
