set(time_engine_SRCS
    timeengine.cpp
    timesource.cpp
    solarsystem.cpp
)

kde4_add_plugin(plasma_engine_time ${time_engine_SRCS})
target_link_libraries(plasma_engine_time ${KDE4_KDECORE_LIBS} ${KDE4_PLASMA_LIBS} ${KDE4_SOLID_LIBS})

install(TARGETS plasma_engine_time DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-time.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )

