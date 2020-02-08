set(keystate_engine_SRCS
    keystate.cpp
    keyservice.cpp
)

kde4_add_plugin(plasma_engine_keystate ${keystate_engine_SRCS})
target_link_libraries(plasma_engine_keystate ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})

install(TARGETS plasma_engine_keystate DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-keystate.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )
install(FILES modifierkeystate.operations DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/services)

