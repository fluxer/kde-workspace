set(kioexec_SRCS main.cpp )

add_executable(kioexec ${kioexec_SRCS})

target_link_libraries(kioexec ${KDE4_KIO_LIBS})

install(
    TARGETS kioexec
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)
