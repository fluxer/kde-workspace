
include_directories( ${CMAKE_SOURCE_DIR}/kscreensaver/libkscreensaver   )


########### next target ###############

set(krandom.kss_SRCS random.cpp)


add_executable(krandom.kss ${krandom.kss_SRCS})

target_link_libraries(krandom.kss ${KDE4_KDEUI_LIBS} m)

if (Q_WS_X11)
  target_link_libraries(krandom.kss ${X11_X11_LIB})
endif (Q_WS_X11)

install(TARGETS krandom.kss ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES krandom.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers )

