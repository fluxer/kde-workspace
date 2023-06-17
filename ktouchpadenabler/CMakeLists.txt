include_directories (${X11_INCLUDE_DIR})

set(kded_ktouchpadenabler_SRCS
    ktouchpadenabler_daemon.cpp
)

kde4_add_kcfg_files(kded_ktouchpadenabler_SRCS settings.kcfgc )

kde4_add_plugin(kded_ktouchpadenabler ${kded_ktouchpadenabler_SRCS})

target_link_libraries(kded_ktouchpadenabler 
    KDE4::kdecore
    KDE4::kdeui
    ${X11_X11_LIB}
    ${X11_Xinput_LIB}
)

install(TARGETS kded_ktouchpadenabler DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES ktouchpadenabler.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)

