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

add_executable( kio_mtp ${kio_mtp_PART_SRCS} )
target_link_libraries( kio_mtp KDE4::kio ${MTP_LIBRARIES} KDE4::solid)

install( TARGETS kio_mtp DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR})


########### install files ###############

install( FILES solid_mtp.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/solid/actions )
install( FILES mtp.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES mtp-network.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/remoteview )
