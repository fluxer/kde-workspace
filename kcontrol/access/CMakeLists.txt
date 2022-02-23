########### next target ###############

kde4_add_plugin(kcm_access kcmaccess.cpp)

target_link_libraries(kcm_access
    ${KDE4_KIO_LIBS}
    ${KDE4_KNOTIFYCONFIG_LIBS}
    ${X11_LIBRARIES}
)

install(
    TARGETS kcm_access
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)


########### next target ###############

set(kaccess_SRCS
    kaccess.cpp
    main.cpp
)

add_executable(kaccess ${kaccess_SRCS})

target_link_libraries(kaccess
    ${KDE4_KDEUI_LIBS}
    ${X11_LIBRARIES}
)

install(
    TARGETS kaccess
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

########### install files ###############

install(
    FILES
    kcmaccess.desktop
    kaccess.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES kaccess.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kaccess
)
