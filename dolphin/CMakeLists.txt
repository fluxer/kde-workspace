include(CheckStructHasMember)

check_struct_has_member("struct dirent" d_type dirent.h HAVE_DIRENT_D_TYPE)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config-dolphin.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-dolphin.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(plugins)
