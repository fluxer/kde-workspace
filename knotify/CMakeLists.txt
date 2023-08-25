project(knotify)

add_subdirectory(sounds)
add_subdirectory(kcm)

install(
    FILES kde.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)