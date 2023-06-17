########### next target ###############

set(kcm_clock_PART_SRCS
    dtime.cpp
    main.cpp
)
kde4_add_plugin(kcm_clock ${kcm_clock_PART_SRCS})

target_link_libraries(kcm_clock
    KDE4::kio
    KDE4::plasma
)

install(
    TARGETS kcm_clock
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### next target ###############

add_executable(kcmdatetimehelper helper.cpp)

target_link_libraries(kcmdatetimehelper KDE4::kdecore)

install(
    TARGETS kcmdatetimehelper
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

kde4_install_auth_helper_files(
    kcmdatetimehelper
    org.kde.kcontrol.kcmclock
    root
)

########### install files ###############

install(
    FILES clock.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
