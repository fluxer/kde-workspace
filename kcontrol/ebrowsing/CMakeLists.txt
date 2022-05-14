kde4_add_plugin(kcm_kurifilt main.cpp)

target_link_libraries(kcm_kurifilt
    ${KDE4_KIO_LIBS}
)

install(
    TARGETS kcm_kurifilt
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES ebrowsing.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
