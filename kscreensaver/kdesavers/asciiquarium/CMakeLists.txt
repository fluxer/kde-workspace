set(kdeasciiquarium_SRCS
    aasaver.cpp
    frame.cpp
    screen.cpp
    sprite.cpp
    settingswidget.ui
)

kde4_add_kcfg_files(kdeasciiquarium_SRCS AASaverConfig.kcfgc)
add_executable(kdeasciiquarium.kss ${kdeasciiquarium_SRCS})

target_link_libraries(kdeasciiquarium.kss ${KDE4_KDEUI_LIBS}  kscreensaver)
install(TARGETS kdeasciiquarium.kss ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES asciiquarium.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers)
