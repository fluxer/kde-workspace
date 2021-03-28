project(kwallet)

add_subdirectory( icons )
add_subdirectory( src )

########### install files ###############

install(
    PROGRAMS
    kwalletmanager.desktop
    kwalletmanager-kwalletd.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
