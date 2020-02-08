set(tasks_engine_SRCS
    tasksengine.cpp
    tasksource.cpp
    taskservice.cpp
    taskjob.cpp
    virtualdesktopssource.cpp
)

kde4_add_plugin(plasma_engine_tasks ${tasks_engine_SRCS})
target_link_libraries(plasma_engine_tasks ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} taskmanager)

install(TARGETS plasma_engine_tasks DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-tasks.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(FILES tasks.operations DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/services)
