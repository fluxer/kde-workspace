set(homerunviewer_SRCS
    ${PROJECT_SOURCE_DIR}/internal/aboutdata.cpp
    fullview.cpp
    main.cpp
    )

include_directories(
    ${PROJECT_SOURCE_DIR}/internal
    )

qt4_add_dbus_adaptor(homerunviewer_SRCS org.kde.homerunviewer.xml
   fullview.h FullView)

add_executable(homerunviewer ${homerunviewer_SRCS})

target_link_libraries(homerunviewer
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECLARATIVE_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
)


install(TARGETS homerunviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES homerunviewer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
