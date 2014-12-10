project(kpartsaver)


include_directories(${KDE4WORKSPACE_INCLUDE_DIR})

set(kpartsaver.kss_SRCS kpartsaver.cpp )

kde4_add_ui_files(kpartsaver.kss_SRCS configwidget.ui )

kde4_add_executable(kpartsaver.kss ${kpartsaver.kss_SRCS})

target_link_libraries(kpartsaver.kss  ${KDE4_KPARTS_LIBS} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})

install(TARGETS kpartsaver.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES kpartsaver.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

