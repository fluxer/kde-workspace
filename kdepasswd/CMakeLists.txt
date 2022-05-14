include_directories(
    ${CMAKE_SOURCE_DIR}/libs/konq
    ${CMAKE_BINARY_DIR}/libs/konq
)

add_subdirectory(kcm)

set(kdepasswd_SRCS
    kdepasswd.cpp
    passwd.cpp
    passwddlg.cpp
    process.cpp
)

add_executable(kdepasswd ${kdepasswd_SRCS})

target_link_libraries(kdepasswd
    ${KDE4_KIO_LIBS}
    ${KDE4_KPTY_LIBS}
)

install(
    TARGETS kdepasswd
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
install(
    PROGRAMS kdepasswd.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
