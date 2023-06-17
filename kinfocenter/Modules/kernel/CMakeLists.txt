include_directories(${KMOD_INCLUDE_DIR})

########### next target ###############

set(kcm_kernel_PART_SRCS kernel.cpp )

kde4_add_plugin(kcm_kernel ${kcm_kernel_PART_SRCS})

target_link_libraries(kcm_kernel
    KDE4::kio
    ${KMOD_LIBRARIES}
)

install(
    TARGETS kcm_kernel
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kernel.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
