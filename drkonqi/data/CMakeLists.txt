install(
    FILES crash.png
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/drkonqi/pics
)
install(
    DIRECTORY debuggers
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/drkonqi
    PATTERN ".svn" EXCLUDE
)
