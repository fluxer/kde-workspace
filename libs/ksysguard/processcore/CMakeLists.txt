
########### next target ###############

set(ksysguard_LIB_SRCS
    processes.cpp
    process.cpp
    processes_local_p.cpp
    processes_remote_p.cpp
    processes_base_p.cpp
)

add_library(processcore SHARED ${ksysguard_LIB_SRCS})

target_link_libraries(processcore PUBLIC
    KDE4::kdecore
)
if(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    message(STATUS "Adding kvm library on NetBSD")
    target_link_libraries(processcore PRIVATE kvm)
endif()

set_target_properties(processcore PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS processcore
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

########### install files ###############

install(
    FILES
    processes.h
    process.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/ksysguard
)

#------ KAuth stuff

# Auth example helper
set(ksysguardprocesslist_helper_srcs
    helper.cpp
    process.cpp
    processes_local_p.cpp
    processes_base_p.cpp
)

add_executable(ksysguardprocesslist_helper ${ksysguardprocesslist_helper_srcs})
target_link_libraries(ksysguardprocesslist_helper PUBLIC
    KDE4::kdecore
)
if(CMAKE_SYSTEM_NAME MATCHES "NetBSD" )
    message(STATUS "Adding kvm library on NetBSD")
    target_link_libraries(ksysguardprocesslist_helper PRIVATE kvm)
endif()

install(
    TARGETS ksysguardprocesslist_helper
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

kde4_install_auth_helper_files(ksysguardprocesslist_helper org.kde.ksysguard.processlisthelper root)
