macro_optional_find_package(Eigen3)
macro_log_feature(EIGEN3_FOUND "Eigen3" "A C++ template library for linear algebra" "kdesupport" FALSE "" "Needed to build the krotation and kpendulum screensavers" )
add_definitions(-DEIGEN2_SUPPORT)

macro_optional_find_package(Kexiv2)
macro_log_feature(KEXIV2_FOUND "Kexiv2" "A C++ library for exiv manipulation" "kdegraphics" FALSE "" "Needed to rotate photos with exif data" )

option(KSCREENSAVER_SOUND_SUPPORT "Build with sound support" OFF)

include_directories(${KDE4WORKSPACE_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

add_subdirectory( data )
macro_optional_add_subdirectory( asciiquarium )

SET(LIBMATH "m")

########### next target ###############

set(kbanner.kss_SRCS banner.cpp )

kde4_add_executable(kbanner.kss ${kbanner.kss_SRCS})

target_link_libraries(kbanner.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kbanner.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kpolygon.kss_SRCS polygon.cpp )

kde4_add_executable(kpolygon.kss ${kpolygon.kss_SRCS})

target_link_libraries(kpolygon.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kpolygon.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kslideshow.kss_SRCS slideshow.cpp )

kde4_add_ui_files(kslideshow.kss_SRCS slideshowcfg.ui )

kde4_add_executable(kslideshow.kss ${kslideshow.kss_SRCS})

macro_bool_to_01(KEXIV2_FOUND HAVE_KEXIV2)

if(HAVE_KEXIV2)
    include_directories( ${KEXIV2_INCLUDES} )
    set_source_files_properties(slideshow.cpp PROPERTIES COMPILE_FLAGS -DHAVE_KEXIV2)
    target_link_libraries(kslideshow.kss  ${KDE4_KIO_LIBS}  ${LIBMATH}  kscreensaver ${KEXIV2_LIBRARIES})
else(HAVE_KEXIV2)
    target_link_libraries(kslideshow.kss  ${KDE4_KIO_LIBS}  ${LIBMATH}  kscreensaver)
endif(HAVE_KEXIV2)

install(TARGETS kslideshow.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(klines.kss_SRCS lines.cpp )

kde4_add_executable(klines.kss ${klines.kss_SRCS})

target_link_libraries(klines.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS klines.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(klorenz.kss_SRCS lorenz.cpp )

kde4_add_executable(klorenz.kss ${klorenz.kss_SRCS})

target_link_libraries(klorenz.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS klorenz.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kblob.kss_SRCS blob.cpp )

kde4_add_executable(kblob.kss ${kblob.kss_SRCS})

target_link_libraries(kblob.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS kblob.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kvm.kss_SRCS kvm.cpp vm.c vm_random.c )

kde4_add_executable(kvm.kss ${kvm.kss_SRCS})

target_link_libraries(kvm.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kvm.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kclock.kss_SRCS kclock.cpp )

kde4_add_executable(kclock.kss ${kclock.kss_SRCS})

target_link_libraries(kclock.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS kclock.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES KClock.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers)


########### next target ###############

#set($(X11_KSAVERS)_SRCS dummy.cpp )
if(Q_WS_X11)
    set(kscience.kss_SRCS science.cpp )

    kde4_add_executable(kscience.kss ${kscience.kss_SRCS})

    target_link_libraries(kscience.kss ${KDE4_KDEUI_LIBS} kscreensaver ${X11_LIBRARIES})

    install(TARGETS kscience.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
    install(FILES KScience.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers)
endif(Q_WS_X11)

########### next target ###############

message(STATUS "readd opengl+sound support screensaver")
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND KSCREENSAVER_SOUND_SUPPORT)
#set(kfiresaver.kss_SRCS firesaverparticle.cpp firesaverwriter.cpp firesaver.cpp )
#kde4_add_ui3_files(kfiresaver.kss_SRCS aversetup.ui)
##kde4_add_executable(kfiresaver.kss ${kfiresaver.kss_SRCS})
#target_link_libraries(kfiresaver.kss  ${KDE4_KDEUI_LIBS} )
#install(TARGETS kfiresaver.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
#install( FILES KFiresaver.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND KSCREENSAVER_SOUND_SUPPORT)

########### next target ###############
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_LIBRARY)

  ## krotation, kpendulum need Eigen2
  if (EIGEN3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIR})

    set(krotation.kss_SRCS rotation.cpp sspreviewarea.cpp)
    kde4_add_ui_files(krotation.kss_SRCS rotationcfg.ui)
    kde4_add_executable(krotation.kss ${krotation.kss_SRCS})
    target_link_libraries(krotation.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
    install(TARGETS krotation.kss ${INSTALL_TARGETS_DEFAULT_ARGS})

    set(kpendulum.kss_SRCS pendulum.cpp sspreviewarea.cpp)
    kde4_add_ui_files(kpendulum.kss_SRCS pendulumcfg.ui)
    kde4_add_executable(kpendulum.kss ${kpendulum.kss_SRCS})
    target_link_libraries(kpendulum.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
    install(TARGETS kpendulum.kss ${INSTALL_TARGETS_DEFAULT_ARGS})
  endif (EIGEN3_FOUND)

  set(ksolarwinds.kss_SRCS SolarWinds.cpp )
  kde4_add_executable(ksolarwinds.kss ${ksolarwinds.kss_SRCS})
  target_link_libraries( ksolarwinds.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS ksolarwinds.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(kflux.kss_SRCS Flux.cpp )
  kde4_add_executable(kflux.kss ${kflux.kss_SRCS})
  target_link_libraries( kflux.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS kflux.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(keuphoria.kss_SRCS Euphoria.cpp )
  kde4_add_executable(keuphoria.kss ${keuphoria.kss_SRCS})
  target_link_libraries( keuphoria.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS keuphoria.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(kfountain.kss_SRCS fountain.cpp )
  kde4_add_ui_files(kfountain.kss_SRCS fountaincfg.ui)
  kde4_add_executable(kfountain.kss ${kfountain.kss_SRCS})
  target_link_libraries( kfountain.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS kfountain.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(kwave.kss_SRCS wave.cpp )
  kde4_add_ui_files(kwave.kss_SRCS wavecfg.ui)
  kde4_add_executable(kwave.kss ${kwave.kss_SRCS})
  target_link_libraries( kwave.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS kwave.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(kgravity.kss_SRCS gravity.cpp )
  kde4_add_ui_files(kgravity.kss_SRCS gravitycfg.ui)
  kde4_add_executable(kgravity.kss ${kgravity.kss_SRCS})
  target_link_libraries( kgravity.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} kscreensaver)
  install(TARGETS kgravity.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  install( FILES KGravity.desktop KWave.desktop KFountain.desktop KFlux.desktop KEuphoria.desktop KSolarWinds.desktop KRotation.desktop KPendulum.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_LIBRARY)

########### install files ###############

install( FILES kscience.png particle.png image.png  DESTINATION  ${DATA_INSTALL_DIR}/kscreensaver )
install( FILES KBanner.desktop KPolygon.desktop KSlideshow.desktop KLines-saver.desktop KLorenz.desktop KBlob.desktop KVm.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

