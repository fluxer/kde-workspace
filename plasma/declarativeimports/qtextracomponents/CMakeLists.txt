project(qtextracomponents)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(qtextracomponents_SRCS
    qtextracomponentsplugin.cpp
    qpixmapitem.cpp
    qimageitem.cpp
    qiconitem.cpp
    mouseeventlistener.cpp
    columnproxymodel.cpp
    )

INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${KDE4_INCLUDES}
)

add_library(qtextracomponentsplugin SHARED ${qtextracomponents_SRCS})
target_link_libraries(qtextracomponentsplugin ${KDE4_KDEUI_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS qtextracomponentsplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/qtextracomponents)

install(FILES qmldir DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/qtextracomponents)
