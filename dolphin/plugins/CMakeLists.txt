include_directories(
    ${CMAKE_SOURCE_DIR}/libs/konq
    ${CMAKE_BINARY_DIR}/libs/konq
)

if (LIBGIT2_FOUND)
    add_subdirectory(git)
endif()
