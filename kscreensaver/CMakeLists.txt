project(kscreensaver)

set(kded_kscreensaver_SRCS
    kded_kscreensaver.cpp
    kscreensaver.cpp
)

qt4_add_dbus_adaptor(kded_kscreensaver_SRCS
    org.freedesktop.ScreenSaver.xml
    kscreensaver.h
    KScreenSaver
)

set(kded_kscreensaver_SRCS 
    ${kded_kscreensaver_SRCS}
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kscreensaver.xml
)

qt4_generate_dbus_interface(kded_kscreensaver.h org.kde.kscreensaver.xml)

kde4_add_plugin(kded_kscreensaver ${kded_kscreensaver_SRCS})
target_link_libraries(kded_kscreensaver PRIVATE
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIDLETIME_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
)

install(
    TARGETS kded_kscreensaver
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kscreensaver.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kscreensaver.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)
