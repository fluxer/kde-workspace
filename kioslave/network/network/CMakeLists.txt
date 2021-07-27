project( molletnetwork )

include_directories(
  builder
)

set( netsystemdriver_SRCS
  builder/simpleitemfactory.cpp
)

set( dnssdnetworkbuilder_SRCS
  builder/dnssd/dnssdnetworkbuilder.cpp
)

set( upnpnetworkbuilder_SRCS
  builder/upnp/cagibidevice.cpp
  builder/upnp/cagibidbuscodec.cpp
  builder/upnp/upnpnetworkbuilder.cpp
)

set( networkbuilder_SRCS
  builder/abstractnetworkbuilder.cpp
  builder/abstractnetsystemfactory.cpp
)

set( networkdbus_LIB_SRCS
  networkdbus.cpp
)

set( molletnetwork_LIB_SRCS
  ${netsystemdriver_SRCS}
  ${upnpnetworkbuilder_SRCS}
  ${dnssdnetworkbuilder_SRCS}
  ${networkbuilder_SRCS}
  ${networkdbus_LIB_SRCS}
  network_p.cpp
  network.cpp
  netdevice_p.cpp
  netdevice.cpp
  netservice_p.cpp
  netservice.cpp
)


set( molletnetwork_LINK_LIBS
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTGUI_LIBRARY}
)

add_library( molletnetwork  SHARED ${molletnetwork_LIB_SRCS} )
target_link_libraries( molletnetwork  ${molletnetwork_LINK_LIBS} )

set_target_properties(molletnetwork PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(molletnetwork)

install( TARGETS molletnetwork  ${INSTALL_TARGETS_DEFAULT_ARGS} )
