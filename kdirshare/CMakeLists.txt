add_subdirectory(filepropertiesplugin)
add_subdirectory(kded)

install(
    FILES kdirshare.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kdirshare
)
