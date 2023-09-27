project(plasma-applet-tasks)

include_directories(
    # for taskmanager_export.h
    ${CMAKE_BINARY_DIR}/libs
    ${CMAKE_BINARY_DIR}/libs/taskmanager
)

set(tasks_SRCS
    tasks.cpp
)

kde4_add_plugin(plasma_applet_tasks ${tasks_SRCS})
target_link_libraries(plasma_applet_tasks
    KDE4::plasma
    taskmanager
)

install(
    TARGETS plasma_applet_tasks
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-tasks.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
