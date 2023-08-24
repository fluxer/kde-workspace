########### next target ###############

set(kded_kcrash_SRCS 
    kded_kcrash.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kcrash.xml
)

qt4_generate_dbus_interface(kded_kcrash.h org.kde.kcrash.xml)

kde4_add_plugin(kded_kcrash ${kded_kcrash_SRCS})
target_link_libraries(kded_kcrash PRIVATE
    ${QT_QTDBUS_LIBRARY}
    KDE4::kdecore
    KDE4::kdeui
)

install(
    TARGETS kded_kcrash
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcrash.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kcrash.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)

install(
    FILES kcrash.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)
