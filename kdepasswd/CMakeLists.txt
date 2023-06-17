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
    KDE4::kio
    KDE4::kpty
)

install(
    TARGETS kdepasswd
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
install(
    PROGRAMS kdepasswd.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
