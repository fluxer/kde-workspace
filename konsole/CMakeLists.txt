project(Konsole)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/libs/konq
    ${CMAKE_SOURCE_DIR}/libs/konq
)

add_definitions(
    ${QT_QTDBUS_DEFINITIONS}
    -DKDE_DEFAULT_DEBUG_AREA=1211
)

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(desktop)
