project(knotify)

add_subdirectory(sounds)

install(
    FILES kde.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)