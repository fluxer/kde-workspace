########### next target ###############

set(kcm_clock_PART_SRCS
    dtime.cpp
    main.cpp
)
kde4_add_plugin(kcm_clock ${kcm_clock_PART_SRCS})

target_link_libraries(kcm_clock
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
)

install(
    TARGETS kcm_clock
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### next target ###############

add_executable(kcmdatetimehelper helper.cpp)

target_link_libraries(kcmdatetimehelper ${KDE4_KDECORE_LIBS})

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
