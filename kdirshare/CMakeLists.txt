add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1600)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(filepropertiesplugin)
add_subdirectory(kded)

install(
    FILES kdirshare.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kdirshare
)
