########### next target ###############

set(kio_sftp_PART_SRCS
   kio_sftp.cpp
)

include_directories(${LIBSSH_INCLUDE_DIR})

add_executable(kio_sftp ${kio_sftp_PART_SRCS})
target_link_libraries(kio_sftp KDE4::kio ${LIBSSH_LIBRARIES})

install(TARGETS kio_sftp  DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR} )

install( FILES sftp.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

