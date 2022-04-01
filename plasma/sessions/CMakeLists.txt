configure_file(
    kde-plasma.desktop.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/kde-plasma.desktop
)
configure_file(
    kde-plasma-safe.desktop.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/kde-plasma-safe.desktop
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kde-plasma.desktop
    ${CMAKE_CURRENT_BINARY_DIR}/kde-plasma-safe.desktop
    DESTINATION ${KDE4_SHARE_INSTALL_PREFIX}/xsessions
)


install(
    FILES plasma.png
    DESTINATION ${KDE4_SHARE_INSTALL_PREFIX}/pixmaps
)
