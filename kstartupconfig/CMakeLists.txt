########### kstartupconfig ###############

set(kstartupconfig_SRCS kstartupconfig.cpp )

add_executable(kstartupconfig4 ${kstartupconfig_SRCS})

target_link_libraries(kstartupconfig4 ${KDE4_KDECORE_LIBS} )

install(TARGETS kstartupconfig4 ${INSTALL_TARGETS_DEFAULT_ARGS} )

