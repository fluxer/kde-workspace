########### next target ###############

set(kded_kdirshare_SRCS 
    kded_kdirshare.cpp
    kdirshareimpl.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdirshare.xml
)

qt4_generate_dbus_interface(kded_kdirshare.h org.kde.kdirshare.xml )

kde4_add_plugin(kded_kdirshare ${kded_kdirshare_SRCS})
target_link_libraries(kded_kdirshare PRIVATE
    KDE4::kdecore
    KDE4::kdeui
    KDE4::kio
    KDE4::kdnssd
    KDE4::kpasswdstore
)

if(OPENSSL_FOUND)
    target_link_libraries(kded_kdirshare PRIVATE ${OPENSSL_LIBRARIES})
endif()

install(
    TARGETS kded_kdirshare
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kdirshare.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdirshare.xml
    DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR}
)


