project(kgreeter)

find_package(GLIB2)
set_package_properties(GLIB2 PROPERTIES
    DESCRIPTION "Low-level core library that forms the basis for projects such as GTK and GNOME"
    URL "https://gitlab.gnome.org/GNOME/glib"
    TYPE REQUIRED
    PURPOSE "Needed for the LightDM greeter"
)

add_subdirectory(kcm)

include_directories(
    ${LIGHTDM_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
)

add_definitions(
    ${LIGHTDM_DEFINITIONS}
)

add_executable(kgreeter kgreeter.cpp)
target_link_libraries(kgreeter
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${LIGHTDM_LIBRARIES}
    ${GLIB2_LIBRARIES}
    gobject-2.0
)

set_target_properties(kgreeter PROPERTIES
    OUTPUT_NAME lightdm-kgreeter-greeter
)

install(
    TARGETS kgreeter
    DESTINATION ${KDE4_SBIN_INSTALL_DIR}
)

install(
    FILES lightdm-kgreeter-greeter.conf
    DESTINATION ${KDE4_SYSCONF_INSTALL_DIR}/lightdm
)

install(
    FILES 50-lightdm-kgreeter-greeter.conf
    DESTINATION ${KDE4_SHARE_INSTALL_PREFIX}/lightdm/lightdm.conf.d
)

install(
    FILES lightdm-kgreeter-greeter.desktop
    DESTINATION ${KDE4_SHARE_INSTALL_PREFIX}/xgreeters
)
