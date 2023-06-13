########### next target ###############

set(kded_kfreespace_SRCS 
    kded_kfreespace.cpp
    kfreespaceimpl.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kfreespace.xml
)

qt4_generate_dbus_interface(kded_kfreespace.h org.kde.kfreespace.xml )

kde4_add_plugin(kded_kfreespace ${kded_kfreespace_SRCS})
target_link_libraries(kded_kfreespace PRIVATE
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_SOLID_LIBS}
)

install(
    TARGETS kded_kfreespace
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kfreespace.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kfreespace.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)


