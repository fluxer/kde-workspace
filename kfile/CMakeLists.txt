set(kfile_SRCS fileprops.cpp )

add_executable(kfile4 ${kfile_SRCS})

target_link_libraries(kfile4 ${KDE4_KIO_LIBS} )

install(TARGETS kfile4 ${INSTALL_TARGETS_DEFAULT_ARGS} )
