########### next target ###############

set(keyboardconfig_SRCS 
    keyboardconfig.cpp
)

kde4_add_plugin(kcm_keyboard ${keyboardconfig_SRCS})

target_link_libraries(kcm_keyboard
    KDE4::kdeui
    KDE4::kcmutils
    ${X11_LIBRARIES}
    ${X11_Xkbfile_LIB}
)

install(
    TARGETS kcm_keyboard
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_keyboard.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
