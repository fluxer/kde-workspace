########### next target ###############

set(kateinterfaces_LIB_SRCS
    kateappadaptor.cpp
    ../interfaces/kate/application.cpp
    ../interfaces/kate/mainwindow.cpp
    ../interfaces/kate/documentmanager.cpp
    ../interfaces/kate/pluginmanager.cpp
    ../interfaces/kate/plugin.cpp
    ../interfaces/kate/pluginconfigpageinterface.cpp
    kateapp.cpp
    kateconfigdialog.cpp
    kateconfigplugindialogpage.cpp
    katedocmanager.cpp
    katemainwindow.cpp
    katepluginmanager.cpp
    kateviewmanager.cpp
    kateviewspace.cpp
    katesavemodifieddialog.cpp
    katemwmodonhddialog.cpp
    katesession.cpp
    katemdi.cpp
    katecontainer.cpp
    katerunninginstanceinfo.cpp
    kateappcommands.cpp
    katequickopen.cpp
)

add_library(kateinterfaces SHARED ${kateinterfaces_LIB_SRCS})

target_link_libraries(kateinterfaces PUBLIC
    ${QT_QTXML_LIBRARY}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KPARTS_LIBS}
)

set_target_properties(kateinterfaces PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

########### next target ###############

add_executable(kate katemain.cpp)

target_link_libraries(kate
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    kateinterfaces
)

generate_export_header(kateinterfaces BASE_NAME kate)

install(
    TARGETS kateinterfaces
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

install(
    TARGETS kate
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/kate_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}
)
