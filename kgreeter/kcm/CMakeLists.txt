########### next target ###############

set(kgreeterconfig_SRCS 
    kgreeterconfig.cpp
    kgreeterconfig.ui
)

kde4_add_plugin(kcm_kgreeterconfig ${kgreeterconfig_SRCS})

target_link_libraries(kcm_kgreeterconfig
    KDE4::kdeui
    KDE4::kcmutils
    KDE4::plasma
)

install(
    TARGETS kcm_kgreeterconfig
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_kgreeterconfig.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

########### next target ###############

add_executable(kcmkgreeterhelper kgreeterhelper.cpp)
target_link_libraries(kcmkgreeterhelper KDE4::kdecore)

install(
    TARGETS kcmkgreeterhelper
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

kde4_install_auth_helper_files(kcmkgreeterhelper org.kde.kcontrol.kcmkgreeter root)
