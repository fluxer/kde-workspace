include_directories(
    # for plasmagenericshell_export.h
    ${CMAKE_BINARY_DIR}/libs/plasmagenericshell
)

add_subdirectory(plasma-desktop)
add_subdirectory(plasma-windowed)
