check_include_files(memory.h HAVE_MEMORY_H)
configure_file (config-xsavers.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xsavers.h )

set(screensaver_base_SRCS main.cpp demowin.cpp saver.cpp helpers.cpp)

########### next target ###############

set(kswarm.kss_SRCS swarm.cpp ${screensaver_base_SRCS} xlock.cpp )


kde4_add_executable(kswarm.kss ${kswarm.kss_SRCS})

target_link_libraries(kswarm.kss  ${KDE4_KDEUI_LIBS} m ${X11_X11_LIB} )

install(TARGETS kswarm.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES KSwarm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

########### next target ###############

if(COMPILE_GL_XSAVERS)
set(kspace_SRCS space.cpp ${screensaver_base_SRCS} xlock.cpp )


kde4_add_executable(kspace.kss ${kspace_SRCS})

target_link_libraries(kspace.kss  ${KDE4_KDEUI_LIBS} ${OPENGL_LIBRARY} ${X11_X11_LIB} )

install(TARGETS kspace.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES KSpace.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers )
endif(COMPILE_GL_XSAVERS)

