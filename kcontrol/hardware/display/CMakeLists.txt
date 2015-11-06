########### next target ###############

kde4_add_plugin(kcm_display display.cpp)

target_link_libraries(kcm_display
    ${KDE4_KCMUTILS_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDEUI_LIBS}
)

install(
    TARGETS kcm_display
    DESTINATION ${PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES display.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
