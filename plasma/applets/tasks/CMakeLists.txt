project(plasma-applet-tasks)

set(tasks_SRCS
    tasks.cpp
)

kde4_add_plugin(plasma_applet_tasks ${tasks_SRCS})
target_link_libraries(plasma_applet_tasks
    KDE4::plasma
    kworkspace
)

install(
    TARGETS plasma_applet_tasks
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-tasks.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
