include_directories(${UNIXAUTH_INCLUDE_DIRS} )

check_include_files(paths.h HAVE_PATHS_H)

configure_file(
    config-kcheckpass.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kcheckpass.h
)

set(kcheckpass_SRCS
    kcheckpass.h
    kcheckpass.c
    checkpass_etcpasswd.c
    checkpass_pam.c
    checkpass_shadow.c
)

add_executable(kcheckpass ${kcheckpass_SRCS})

add_definitions(-U_REENTRANT)
target_link_libraries(kcheckpass
    ${UNIXAUTH_LIBRARIES}
    ${SOCKET_LIBRARIES}
)

install(
    TARGETS kcheckpass
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)
install(CODE "
    set(KCP_PATH \"\$ENV{DESTDIR}${KDE4_LIBEXEC_INSTALL_DIR}/kcheckpass\")
    EXECUTE_PROCESS(COMMAND sh -c \"chown root '\${KCP_PATH}' && chmod +s '\${KCP_PATH}'\")
")
