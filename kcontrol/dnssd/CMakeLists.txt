########### next target ###############

kde4_add_plugin(kcm_kdnssd kcmdnssd.cpp)

target_link_libraries(kcm_kdnssd
    ${KDE4_KDNSSD_LIBS}
    ${KDE4_KDEUI_LIBS}
)

install(
    TARGETS kcm_kdnssd
    DESTINATION ${PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kcm_kdnssd.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
