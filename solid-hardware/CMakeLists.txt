
########### next target ###############

set(solid-hardware_SRCS solid-hardware.cpp)
add_executable(solid-hardware ${solid-hardware_SRCS})
target_link_libraries(solid-hardware ${KDE4_KDECORE_LIBS} ${KDE4_SOLID_LIBS})
install(TARGETS solid-hardware ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############



