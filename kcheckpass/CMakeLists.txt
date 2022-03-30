include_directories(${UNIXAUTH_INCLUDE_DIRS})

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

set(kcheckpass_suid "")
if (HAVE_GETSPNAM)
    set(kcheckpass_suid "SETUID")
    message(WARNING "Will install kcheckpass SUID")
endif()

install(
    TARGETS kcheckpass
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE ${kcheckpass_suid}
)
