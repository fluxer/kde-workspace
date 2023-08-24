project(kfreespace)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1604)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(kcm)
add_subdirectory(kded)

install(
    FILES kfreespace.notifyrc
    DESTINATION ${KDE4_CONFIG_INSTALL_DIR}/notifications
)
