add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1207)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs/ksysguard
    ${CMAKE_SOURCE_DIR}/libs/ksysguard/processcore
    ${CMAKE_SOURCE_DIR}/libs/plasmagenericshell
    # for plasmagenericshell_export.h
    ${CMAKE_BINARY_DIR}/libs/plasmagenericshell
    # for kworkspace_export.h
    ${CMAKE_BINARY_DIR}/libs/kworkspace
)

set(krunner_KCFG_SRCS kcfg/krunnersettings.kcfgc)
set(krunner_dbusAppXML dbus/org.kde.krunner.App.xml)


set(default_interface_SRCS
    interfaces/default/interface.cpp
    interfaces/default/resultitem.cpp
    interfaces/default/resultscene.cpp
    interfaces/default/resultview.cpp
    interfaces/default/krunnerhistorycombobox.cpp
    interfaces/default/selectionbar.cpp
)

set(quicksand_interface_SRCS
   interfaces/quicksand/qs_completionbox.cpp
   interfaces/quicksand/qs_dialog.cpp
   interfaces/quicksand/qs_matchitem.cpp
   interfaces/quicksand/qs_matchview.cpp
   interfaces/quicksand/qs_queryactionitem.cpp
   interfaces/quicksand/qs_querymatchitem.cpp
   interfaces/quicksand/qs_statusbar.cpp)

set(krunner_SRCS
   krunnerdialog.cpp
   configdialog.cpp
   krunnerapp.cpp
   main.cpp
   ${default_interface_SRCS}
   ${quicksand_interface_SRCS}
)

kde4_add_kcfg_files(krunner_SRCS ${krunner_KCFG_SRCS})

qt4_add_dbus_adaptor(krunner_SRCS ${krunner_dbusAppXML} krunnerapp.h KRunnerApp)

kde4_add_dbus_service(dbus/org.kde.krunner.service.in)

set(krunner_SRCS
    ${krunner_SRCS}
    ksystemactivitydialog.cpp
)

add_executable(krunner ${krunner_SRCS})

target_link_libraries(krunner
    ${KDE4_PLASMA_LIBS}
    ${KDE4_SOLID_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${X11_X11_LIB}
    kworkspace
    plasmagenericshell
    processui
)

if(X11_Xcursor_FOUND)
    target_link_libraries(krunner ${X11_Xcursor_LIB})
endif()

install(
    TARGETS krunner
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    FILES krunner.desktop
    DESTINATION ${KDE4_AUTOSTART_INSTALL_DIR}
)
install(
    FILES ${krunner_dbusAppXML}
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)
