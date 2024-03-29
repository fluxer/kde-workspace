add_definitions(-DKDE_DEFAULT_DEBUG_AREA=7116)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(trashcommon_PART_SRCS
    trashimpl.cpp
    discspaceutil.cpp
    trashsizecache.cpp
)

########### next target ###############

set(kio_trash_PART_SRCS kio_trash.cpp ${trashcommon_PART_SRCS})

add_executable(kio_trash ${kio_trash_PART_SRCS})

target_link_libraries(kio_trash
    KDE4::kio
    KDE4::solid
)

install(
    TARGETS kio_trash
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

########### next target ###############

set(ktrash_SRCS ktrash.cpp)

add_executable(ktrash ${ktrash_SRCS})

target_link_libraries(ktrash KDE4::kio)

install(
    TARGETS ktrash
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### next target ###############

set(kcm_trash_PART_SRCS kcmtrash.cpp ${trashcommon_PART_SRCS})
kde4_add_plugin(kcm_trash ${kcm_trash_PART_SRCS})
target_link_libraries(kcm_trash
    KDE4::kdeui
    KDE4::kio
    KDE4::solid
)

install(
    TARGETS kcm_trash
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES trash.protocol
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES kcmtrash.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
