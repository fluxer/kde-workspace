
########### next target ###############

set(kio_floppy_PART_SRCS kio_floppy.cpp program.cpp )

add_executable(kio_floppy ${kio_floppy_PART_SRCS})

target_link_libraries(kio_floppy  KDE4::kio )

install(TARGETS kio_floppy  DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR} )


########### install files ###############

install( FILES floppy.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

