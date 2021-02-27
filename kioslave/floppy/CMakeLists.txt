find_program(MTOOLS_EXECUTABLES NAMES mdir mmd mdel mrd mren mcopy)
add_feature_info(
    "mtools"
    MTOOLS_EXECUTABLES
    "KIO floppy support"
)

########### next target ###############

set(kio_floppy_PART_SRCS kio_floppy.cpp program.cpp )


kde4_add_plugin(kio_floppy ${kio_floppy_PART_SRCS})


target_link_libraries(kio_floppy  ${KDE4_KIO_LIBS} )

install(TARGETS kio_floppy  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES floppy.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

