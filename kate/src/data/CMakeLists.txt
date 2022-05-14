# desktop files and co
install(
    PROGRAMS kate.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)

install(
    FILES katerc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}
)

install(
    FILES
    tips
    kateui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate
)

install(
    FILES kateplugin.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)

# kate application icons
add_subdirectory(icons)
