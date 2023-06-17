########### next target ###############

set(kcm_nic_PART_SRCS nic.cpp )

kde4_add_plugin(kcm_nic ${kcm_nic_PART_SRCS})

target_link_libraries(kcm_nic
    KDE4::kio
    ${QT_QTNETWORK_LIBRARY}
)

install(
    TARGETS kcm_nic
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES nic.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
