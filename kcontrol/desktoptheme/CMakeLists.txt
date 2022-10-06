project(desktoptheme)

set(kcmdesktoptheme_SRCS
    kcmdesktoptheme.cpp
    desktopthemedetails.cpp
    thememodel.cpp
)

kde4_add_plugin(kcm_desktoptheme ${kcmdesktoptheme_SRCS})
target_link_libraries(kcm_desktoptheme
    ${X11_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KARCHIVE_LIBS}
)

install(
    TARGETS kcm_desktoptheme
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES desktoptheme.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

