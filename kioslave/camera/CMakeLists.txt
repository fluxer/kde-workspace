include_directories(
    ${GPHOTO2_INCLUDE_DIR}
)

add_executable(kio_camera kio_camera.cpp)

target_link_libraries(kio_camera
    KDE4::kio
    ${GPHOTO2_LIBRARIES}
)

install(
    TARGETS kio_camera
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

install(
    FILES camera.protocol
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES solid_camera.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/solid/actions
)

install(
    FILES camera-network.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/remoteview
)
