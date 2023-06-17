
########### next target ###############

kde4_add_plugin(kcm_kmediaplayer kcmplayer.cpp)

target_link_libraries(kcm_kmediaplayer
    KDE4::kio
    KDE4::kmediaplayer
)

install(TARGETS kcm_kmediaplayer  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES kcmplayer.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})


