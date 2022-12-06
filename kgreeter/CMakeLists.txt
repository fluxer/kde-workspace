project(kgreeter)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1603)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lightdm-kgreeter-greeter.desktop.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lightdm-kgreeter-greeter.desktop
)

find_package(GLIB2)
set_package_properties(GLIB2 PROPERTIES
    DESCRIPTION "Low-level core library that forms the basis for projects such as GTK and GNOME"
    URL "https://gitlab.gnome.org/GNOME/glib"
    TYPE REQUIRED
    PURPOSE "Needed for the LightDM greeter"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIGHTDM_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
)

add_definitions(
    ${LIGHTDM_DEFINITIONS}
)

add_executable(lightdm-kgreeter-greeter kgreeter.cpp)
target_link_libraries(lightdm-kgreeter-greeter
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${LIGHTDM_LIBRARIES}
    ${GLIB2_LIBRARIES}
)

install(
    TARGETS lightdm-kgreeter-greeter
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
    FILES ${CMAKE_CURRENT_BINARY_DIR}/lightdm-kgreeter-greeter.desktop
    DESTINATION ${KDE4_SHARE_INSTALL_PREFIX}/xgreeters
)

add_subdirectory(kcm)

install(
    FILES data/rectangle.png
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kgreeter
)
