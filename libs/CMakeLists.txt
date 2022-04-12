include_directories(
    # for taskmanager_export.h
    ${CMAKE_BINARY_DIR}/libs
)

add_subdirectory(kworkspace)
add_subdirectory(oxygen)
add_subdirectory(plasmaclock)
add_subdirectory(plasmagenericshell)
add_subdirectory(taskmanager)
add_subdirectory(ksysguard)

