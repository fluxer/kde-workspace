check_include_files(utime.h HAVE_UTIME_H)

configure_file(config-smb.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-smb.h)

set(kio_smb_PART_SRCS 
   kio_smb.cpp 
   kio_smb_auth.cpp 
   kio_smb_browse.cpp 
   kio_smb_config.cpp 
   kio_smb_dir.cpp 
   kio_smb_file.cpp 
   kio_smb_internal.cpp 
   kio_smb_mount.cpp
)

include_directories(${SAMBA_INCLUDE_DIR})

kde4_add_plugin(kio_smb ${kio_smb_PART_SRCS})


target_link_libraries(kio_smb  ${KDE4_KIO_LIBS} ${SAMBA_LIBRARIES})

install(TARGETS kio_smb  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES smb.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR})
install( FILES smb-network.desktop  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/remoteview)

