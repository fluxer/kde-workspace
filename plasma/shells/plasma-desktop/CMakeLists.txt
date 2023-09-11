include_directories(
    ${CMAKE_SOURCE_DIR}/libs 
    ${CMAKE_SOURCE_DIR}/libs/plasmagenericshell
    ${CMAKE_SOURCE_DIR}/plasma/desktop/shell/scripting
)

set(plasma_SRCS
    scripting/desktopscriptengine.cpp
    scripting/panel.cpp
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
)

set(plasmaapp_dbusXML dbus/org.kde.plasma.App.xml)
qt4_add_dbus_adaptor(plasma_SRCS ${plasmaapp_dbusXML} plasmaapp.h PlasmaApp)

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
    FILES data/plasma-desktop-js.xml
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/katepart/syntax/
)

install(
    FILES data/plasma-desktoprc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}
)
