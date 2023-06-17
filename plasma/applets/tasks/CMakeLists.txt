project(plasma-tasks)

include_directories(
    # for taskmanager_export.h
    ${CMAKE_BINARY_DIR}/libs
    ${CMAKE_BINARY_DIR}/libs/taskmanager
)

set(tasks_SRCS
    tasks.cpp
    support/draghelper.cpp
    support/textlabel.cpp
    support/declarativeitemcontainer.cpp
    support/tooltip.cpp
    tasksConfig.ui
)

kde4_add_plugin(plasma_applet_tasks ${tasks_SRCS})

target_link_libraries(plasma_applet_tasks
    KDE4::kdeui
    KDE4::plasma
    ${QT_QTDECLARATIVE_LIBRARY}
    taskmanager
)

install(TARGETS plasma_applet_tasks DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-tasks-default.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

install(DIRECTORY package/ DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/packages/org.kde.plasma.tasks)
