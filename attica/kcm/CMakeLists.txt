set(kcm_attica_PART_SRCS
    providerconfigwidget.cpp
    atticamodule.cpp
    providermanagement.ui
    providerconfigwidget.ui
)

kde4_add_plugin(kcm_attica ${kcm_attica_PART_SRCS})

target_link_libraries(kcm_attica
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${LIBATTICA_LIBRARIES}
)

install(TARGETS kcm_attica DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES kcm_attica.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
