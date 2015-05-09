########### next target ###############

set(kcminit_SRCS main.cpp)


kde4_add_executable(kcminit ${kcminit_SRCS})

target_link_libraries(kcminit  ${KDE4_KCMUTILS_LIBS} ${KDE4_KDEUI_LIBS} ${X11_LIBRARIES})

install(TARGETS kcminit         ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### next target ###############

# TODO might be simpler to make <whatever>_startup to be a symlink to <whatever>

set(kcminit_startup_SRCS main.cpp)


kde4_add_executable( kcminit_startup ${kcminit_startup_SRCS})

target_link_libraries(kcminit_startup  ${KDE4_KCMUTILS_LIBS} ${KDE4_KDEUI_LIBS} ${X11_LIBRARIES})

install(TARGETS kcminit_startup         ${INSTALL_TARGETS_DEFAULT_ARGS} )

