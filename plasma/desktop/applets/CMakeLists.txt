add_subdirectory(kickoff)
add_subdirectory(trash)
if(LIBKONQ_FOUND)
    add_subdirectory(homerun)
    add_subdirectory(folderview)
endif()

#task and windowlist depend on libs/taskmanager
add_subdirectory(tasks)
add_subdirectory(windowlist)
add_subdirectory(pager)
