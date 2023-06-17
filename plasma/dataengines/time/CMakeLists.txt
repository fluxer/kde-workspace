set(time_engine_SRCS
    timeengine.cpp
    timesource.cpp
    solarsystem.cpp
)

kde4_add_plugin(plasma_engine_time ${time_engine_SRCS})
target_link_libraries(plasma_engine_time KDE4::kdecore KDE4::plasma KDE4::solid)

install(TARGETS plasma_engine_time DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-time.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )

