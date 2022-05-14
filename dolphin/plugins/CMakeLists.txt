include_directories(
    ${CMAKE_SOURCE_DIR}/libs/konq
    ${CMAKE_BINARY_DIR}/libs/konq
)

add_subdirectory(svn)
add_subdirectory(git)
add_subdirectory(hg)
add_subdirectory(bazaar)
add_subdirectory(dropbox)
