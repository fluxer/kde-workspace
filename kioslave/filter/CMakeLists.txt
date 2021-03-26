
########### next target ###############

set(kio_filter_PART_SRCS filter.cc)


kde4_add_plugin(kio_filter ${kio_filter_PART_SRCS})


target_link_libraries(kio_filter  ${KDE4_KIO_LIBS})

install(TARGETS kio_filter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install(
    FILES gzip.protocol
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

if(BZIP2_FOUND)
    install(
        FILES bzip.protocol bzip2.protocol
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
    )
endif()

if(LIBLZMA_FOUND)
    install(
        FILES lzma.protocol xz.protocol
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
    )
endif()
