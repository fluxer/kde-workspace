project(KMail)

add_executable(kmail main.cpp)

target_link_libraries(kmail
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KEMAIL_LIBS}
)

install(
    TARGETS kmail
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    PROGRAMS kmail.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
