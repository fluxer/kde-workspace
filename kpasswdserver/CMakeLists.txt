if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(kded_kpasswdserver_SRCS kpasswdserver.cpp )

qt4_add_dbus_adaptor(kded_kpasswdserver_SRCS org.kde.KPasswdServer.xml kpasswdserver.h KPasswdServer)

kde4_add_plugin(kded_kpasswdserver ${kded_kpasswdserver_SRCS})

target_link_libraries(kded_kpasswdserver ${KDE4_KIO_LIBS})

install(TARGETS kded_kpasswdserver  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install( FILES kpasswdserver.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/kded )
