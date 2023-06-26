include_directories(
    ${CMAKE_SOURCE_DIR}/libs 
    ${CMAKE_SOURCE_DIR}/libs/plasmagenericshell
    ${CMAKE_SOURCE_DIR}/plasma/desktop/shell/scripting
)

set(plasma_SRCS
    scripting/desktopscriptengine.cpp
    scripting/panel.cpp
    checkbox.cpp
    controllerwindow.cpp
    desktopcorona.cpp
    desktopview.cpp
    desktoptracker.cpp
    interactiveconsole.cpp
    main.cpp
    panelcontroller.cpp
    panelview.cpp
    panelapplethandle.cpp
    panelappletoverlay.cpp
    plasmaapp.cpp
    positioningruler.cpp
    klistconfirmationdialog.cpp
)

kde4_add_kcfg_files(plasma_SRCS data/plasma-shell-desktop.kcfgc)

set(plasmaapp_dbusXML dbus/org.kde.plasma.App.xml)
qt4_add_dbus_adaptor(plasma_SRCS ${plasmaapp_dbusXML} plasmaapp.h PlasmaApp)

set(ksmserver_xml ${CMAKE_SOURCE_DIR}/ksmserver/org.kde.KSMServerInterface.xml)
qt4_add_dbus_interface(plasma_SRCS ${ksmserver_xml} ksmserver_interface)

add_executable(plasma-desktop ${plasma_SRCS})

target_link_libraries(plasma-desktop
    KDE4::plasma
    KDE4::kio
    KDE4::kfile
    KDE4::ktexteditor
    KDE4::kidletime
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${X11_LIBRARIES}
    kworkspace
    plasmagenericshell
)

install(
    TARGETS plasma-desktop
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
install(
    FILES data/plasma-desktop.desktop
    DESTINATION ${KDE4_AUTOSTART_INSTALL_DIR}
)
install(
    FILES data/plasma-shell-desktop.kcfg
    DESTINATION ${KDE4_KCFG_INSTALL_DIR}
)
install(
    FILES data/plasma-desktop-js.xml
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/katepart/syntax/
)
install(
    FILES data/plasma-desktoprc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}
)
