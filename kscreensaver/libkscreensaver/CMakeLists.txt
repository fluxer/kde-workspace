



########### next target ###############

set(kscreensaver_LIB_SRCS main.cpp kscreensaver.cpp)


add_library(kscreensaver SHARED ${kscreensaver_LIB_SRCS})

target_link_libraries(kscreensaver  ${KDE4_KIO_LIBS} ${X11_X11_LIB})
set_target_properties(kscreensaver PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
set_target_properties(kscreensaver PROPERTIES DEFINE_SYMBOL MAKE_KSCREENSAVER_LIB)

install(TARGETS kscreensaver EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES screensaver.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})

########### install files ###############

install( FILES kscreensaver.h kscreensaver_vroot.h kscreensaver_export.h DESTINATION ${KDE4_INCLUDE_INSTALL_DIR} COMPONENT Devel)

