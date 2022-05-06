########### next target ###############

set(kfirewallconfig_SRCS 
    kfirewallconfig.cpp
    kfirewallconfig.ui
)

kde4_add_plugin(kcm_kfirewallconfig ${kfirewallconfig_SRCS})

target_link_libraries(kcm_kfirewallconfig
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KCMUTILS_LIBS}
)

install(
    TARGETS kcm_kfirewallconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_kfirewallconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

########### next target ###############

add_executable(kcmkfirewallhelper kfirewallhelper.cpp)
target_link_libraries(kcmkfirewallhelper
    ${KDE4_KDECORE_LIBS}
)

install(
    TARGETS kcmkfirewallhelper
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

kde4_install_auth_helper_files(kcmkfirewallhelper org.kde.kcontrol.kcmkfirewall root)
