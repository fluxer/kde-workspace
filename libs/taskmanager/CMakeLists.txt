add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1204)

########### next target ###############

set(taskmanager_LIB_SRCS
    abstractgroupableitem.cpp
    abstractgroupingstrategy.cpp
    abstractsortingstrategy.cpp
    groupmanager.cpp
    launcheritem.cpp
    startup.cpp
    strategies/alphasortingstrategy.cpp
    strategies/desktopsortingstrategy.cpp
    strategies/programgroupingstrategy.cpp
    strategies/manualgroupingstrategy.cpp
    strategies/manualsortingstrategy.cpp
    task.cpp
    taskactions.cpp
    taskgroup.cpp
    taskitem.cpp
    taskmanager.cpp
    tasksmodel.cpp
    launcherconfig.cpp
    launcherproperties.cpp
    launcherconfig.ui
    launcherproperties.ui
)

set(taskmanager_LIB_SRCS ${taskmanager_LIB_SRCS} task_x11.cpp)

add_library(taskmanager SHARED ${taskmanager_LIB_SRCS})

target_link_libraries(taskmanager PUBLIC
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    processcore
)

target_link_libraries(taskmanager PRIVATE
    ${X11_X11_LIB}
)

set_target_properties(taskmanager PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(taskmanager)

install(
    TARGETS taskmanager
    EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES
    abstractgroupableitem.h
    abstractgroupingstrategy.h
    abstractsortingstrategy.h
    groupmanager.h
    launcheritem.h
    startup.h
    task.h
    taskactions.h
    taskgroup.h
    taskitem.h
    taskmanager.h
    ${CMAKE_CURRENT_BINARY_DIR}/taskmanager_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/taskmanager COMPONENT Devel
)


#set(tasksmodel_SRCS 
#   modeltest/dynamictreemodel.cpp
#   modeltest/modeltest.cpp
#   modeltest/main.cpp
#)

#add_executable(tasksmodeltest ${tasksmodel_SRCS})
#target_link_libraries(tasksmodeltest ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${X11_LIBRARIES} taskmanager)


