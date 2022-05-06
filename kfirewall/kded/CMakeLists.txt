########### next target ###############

set(kded_kfirewall_SRCS 
    kded_kfirewall.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kfirewall.xml
)

qt4_generate_dbus_interface(kded_kfirewall.h org.kde.kfirewall.xml )

kde4_add_plugin(kded_kfirewall ${kded_kfirewall_SRCS})
target_link_libraries(kded_kfirewall PRIVATE
    ${KDE4_KDECORE_LIBS}
)

install(
    TARGETS kded_kfirewall
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kfirewall.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kfirewall.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)


