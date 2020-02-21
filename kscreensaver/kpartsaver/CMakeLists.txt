project(kpartsaver)

set(kpartsaver.kss_SRCS kpartsaver.cpp configwidget.ui )

add_executable(kpartsaver.kss ${kpartsaver.kss_SRCS})

target_link_libraries(kpartsaver.kss  ${KDE4_KPARTS_LIBS} kscreensaver)

install(TARGETS kpartsaver.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES kpartsaver.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers )

