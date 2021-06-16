#add_definitions(-DKDE_DEFAULT_DEBUG_AREA=7999)
add_definitions(-DQT_NO_CAST_FROM_ASCII)

set( kio_mtp_PART_SRCS
     devicecache.cpp
     filecache.cpp
     kio_mtp.cpp
     kio_mtp_helpers.cpp
)

include_directories(
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
)

kde4_add_plugin( kio_mtp ${kio_mtp_PART_SRCS} )
target_link_libraries( kio_mtp ${KDE4_KIO_LIBS} ${MTP_LIBRARIES} ${KDE4_SOLID_LIBS})

install( TARGETS kio_mtp DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES solid_mtp.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/solid/actions )
install( FILES mtp.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES mtp-network.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/remoteview )
