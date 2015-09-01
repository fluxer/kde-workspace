########### kstartupconfig ###############

set(kstartupconfig_SRCS kstartupconfig.c )

add_executable(kstartupconfig4 ${kstartupconfig_SRCS})

target_link_libraries(kstartupconfig4 ${KDE4_KDEFAKES_LIBS} )

install(TARGETS kstartupconfig4 ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### kdostartupconfig ###############

set(kdostartupconfig_SRCS kdostartupconfig.cpp )

add_executable(kdostartupconfig4 ${kdostartupconfig_SRCS})

target_link_libraries(kdostartupconfig4 ${KDE4_KDECORE_LIBS} )

install(TARGETS kdostartupconfig4 ${INSTALL_TARGETS_DEFAULT_ARGS} )

