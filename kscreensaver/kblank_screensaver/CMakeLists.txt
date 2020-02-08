
include_directories( ${CMAKE_SOURCE_DIR}/kscreensaver/libkscreensaver   )


########### next target ###############

set(kblankscrn.kss_SRCS blankscrn.cpp )


add_executable(kblankscrn.kss ${kblankscrn.kss_SRCS})

target_link_libraries(kblankscrn.kss  ${KDE4_KDEUI_LIBS} kscreensaver m )

install(TARGETS kblankscrn.kss ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES kblank.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers )
