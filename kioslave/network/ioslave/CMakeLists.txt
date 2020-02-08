project( kio_network )

include_directories(
  ../network
)

set( kio_network_SRCS
  networkdbusinterface.cpp
  networkinitwatcher.cpp
  networkthread.cpp
  networkuri.cpp
  mimetypes.cpp
  networkslave.cpp
  main.cpp
)

# qt4_add_dbus_interface( kio_network_SRCS  ../kded/org.kde.network.xml networkdbusproxy )

kde4_add_plugin( kio_network  ${kio_network_SRCS} )

target_link_libraries( kio_network  molletnetwork ${KDE4_KIO_LIBS} )

install( TARGETS kio_network  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install( FILES network.protocol  DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install( FILES network.desktop  DESTINATION ${KDE4_DATA_INSTALL_DIR}/remoteview)
install( FILES virtualfolder_network.desktop  DESTINATION ${KDE4_DATA_INSTALL_DIR}/konqsidebartng/virtual_folders/remote)
