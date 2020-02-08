set(kded_ktimezoned_SRCS ktimezoned.cpp)

kde4_add_plugin(kded_ktimezoned ${kded_ktimezoned_SRCS})

#qt4_add_dbus_adaptor(kded_ktimezoned_SRCS org.kde.KTimeZoned ktimezonedbus.h DBusHandler)

target_link_libraries(kded_ktimezoned ${KDE4_KIO_LIBS})

install(TARGETS kded_ktimezoned DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES ktimezoned.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded )
install( FILES org.kde.KTimeZoned.xml DESTINATION ${KDE4_DBUS_INTERFACES_INSTALL_DIR} )


