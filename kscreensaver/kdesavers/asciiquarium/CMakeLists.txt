set(kdeasciiquarium_SRCS
    aasaver.cpp
    frame.cpp
    screen.cpp
    sprite.cpp
    )

kde4_add_ui_files(kdeasciiquarium_SRCS
    settingswidget.ui
)

kde4_add_kcfg_files(kdeasciiquarium_SRCS AASaverConfig.kcfgc)
kde4_add_executable(kdeasciiquarium.kss ${kdeasciiquarium_SRCS})

target_link_libraries(kdeasciiquarium.kss ${KDE4_KDEUI_LIBS}  ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
install(TARGETS kdeasciiquarium.kss ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES asciiquarium.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers)
