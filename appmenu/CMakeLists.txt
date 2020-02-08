include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(kded_appmenu_SRCS
    menubutton.cpp
    menuwidget.cpp
    menubar.cpp
    topmenubar.cpp
    glowbar.cpp
    verticalmenu.cpp
    shadows.cpp
)

if(DBUSMENUEQT_FOUND)
    set(kded_appmenu_SRCS
        ${kded_appmenu_SRCS}
        appmenu.cpp
        appmenu_dbus.cpp
        menuimporter.cpp
    )

    qt4_add_dbus_adaptor(kded_appmenu_SRCS
        com.canonical.AppMenu.Registrar.xml
        menuimporter.h
        MenuImporter
        menuimporteradaptor
        MenuImporterAdaptor
    )

    qt4_add_dbus_adaptor(kded_appmenu_SRCS
        org.kde.kded.appmenu.xml
        appmenu_dbus.h
        AppmenuDBus
        appmenuadaptor
        AppmenuAdaptor
    )
endif()

kde4_add_plugin(kded_appmenu ${kded_appmenu_SRCS})

target_link_libraries(kded_appmenu
    ${KDE4_KIO_LIBS}
    ${X11_LIBRARIES}
    ${KDE4_PLASMA_LIBS}
)

if(DBUSMENUQT_FOUND)
    target_link_libraries(kded_appmenu ${DBUSMENUQT_LIBRARIES})
    include_directories(${DBUSMENUQT_INCLUDE_DIR})
endif()

install(
    TARGETS kded_appmenu
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES appmenu.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)
install(
    FILES
    com.canonical.AppMenu.Registrar.xml
    org.kde.kded.appmenu.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)
