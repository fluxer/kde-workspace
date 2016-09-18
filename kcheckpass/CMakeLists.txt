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
    checkpass_osfc2passwd.c
    checkpass_aix.c
)

add_executable(kcheckpass ${kcheckpass_SRCS})

if(KDE4_ENABLE_FPIE)
    macro_add_compile_flags(kcheckpass ${KDE4_CXX_FPIE_FLAGS})
    macro_add_link_flags(kcheckpass ${KDE4_PIE_LDFLAGS})
endif(KDE4_ENABLE_FPIE)

add_definitions(-U_REENTRANT)
target_link_libraries(kcheckpass
    ${UNIXAUTH_LIBRARIES}
    ${SOCKET_LIBRARIES}
    # for kdefakes
    ${KDE4_KDECORE_LIBS}
)

install(
    TARGETS kcheckpass
    DESTINATION ${LIBEXEC_INSTALL_DIR}
)
install(CODE "
    set(KCP_PATH \"\$ENV{DESTDIR}${LIBEXEC_INSTALL_DIR}/kcheckpass\")
    EXECUTE_PROCESS(COMMAND sh -c \"chown root '\${KCP_PATH}' && chmod +s '\${KCP_PATH}'\")
")
