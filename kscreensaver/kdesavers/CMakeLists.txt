macro_optional_add_subdirectory( asciiquarium )

SET(LIBMATH "m")

########### next target ###############

set(kbanner.kss_SRCS banner.cpp )

add_executable(kbanner.kss ${kbanner.kss_SRCS})

target_link_libraries(kbanner.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kbanner.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kpolygon.kss_SRCS polygon.cpp )

add_executable(kpolygon.kss ${kpolygon.kss_SRCS})

target_link_libraries(kpolygon.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kpolygon.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kslideshow.kss_SRCS slideshow.cpp slideshowcfg.ui )

add_executable(kslideshow.kss ${kslideshow.kss_SRCS})

set_source_files_properties(slideshow.cpp PROPERTIES COMPILE_FLAGS -DHAVE_KEXIV2)
target_link_libraries(kslideshow.kss
    ${KDE4_KIO_LIBS}
    ${LIBMATH}
    kscreensaver
    ${KDE4_KEXIV2_LIBS}
)

install(TARGETS kslideshow.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(klines.kss_SRCS lines.cpp )

add_executable(klines.kss ${klines.kss_SRCS})

target_link_libraries(klines.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS klines.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(klorenz.kss_SRCS lorenz.cpp )

add_executable(klorenz.kss ${klorenz.kss_SRCS})

target_link_libraries(klorenz.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS klorenz.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kblob.kss_SRCS blob.cpp )

add_executable(kblob.kss ${kblob.kss_SRCS})

target_link_libraries(kblob.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS kblob.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kvm.kss_SRCS kvm.cpp vm.c vm_random.c )

add_executable(kvm.kss ${kvm.kss_SRCS})

target_link_libraries(kvm.kss ${KDE4_KDEUI_LIBS} kscreensaver)

install(TARGETS kvm.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kclock.kss_SRCS kclock.cpp )

add_executable(kclock.kss ${kclock.kss_SRCS})

target_link_libraries(kclock.kss ${KDE4_KDEUI_LIBS} ${LIBMATH} kscreensaver)

install(TARGETS kclock.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES KClock.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers)


########### next target ###############

#set($(X11_KSAVERS)_SRCS dummy.cpp )
if(Q_WS_X11)
    set(kscience.kss_SRCS science.cpp )

    add_executable(kscience.kss ${kscience.kss_SRCS})

    target_link_libraries(kscience.kss ${KDE4_KDEUI_LIBS} kscreensaver ${X11_LIBRARIES})

    install(TARGETS kscience.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
    install(FILES KScience.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers)
endif(Q_WS_X11)

########### install files ###############

install( FILES kscience.png particle.png image.png  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kscreensaver )
install( FILES KBanner.desktop KPolygon.desktop KSlideshow.desktop KLines-saver.desktop KLorenz.desktop KBlob.desktop KVm.desktop DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/ScreenSavers )

