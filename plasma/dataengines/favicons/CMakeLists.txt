set(favicons_engine_SRCS
    favicons.cpp
)

kde4_add_plugin(plasma_engine_favicons ${favicons_engine_SRCS} )
target_link_libraries(plasma_engine_favicons ${KDE4_PLASMA_LIBS} ${KDE4_KDECORE_LIBS})

install(TARGETS plasma_engine_favicons DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install(FILES plasma-dataengine-favicons.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )


