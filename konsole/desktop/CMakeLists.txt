install(
    PROGRAMS konsole.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)

install(
    FILES terminalemulator.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)

install(
    FILES konsolepart.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES
    konsolehere.desktop
    konsolerun.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/ServiceMenus
)

install(
    FILES
    konsoleui.rc
    sessionui.rc
    partui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/konsole
)

install(
    FILES konsole.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)
