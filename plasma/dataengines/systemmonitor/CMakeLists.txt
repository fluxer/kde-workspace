
set(systemmonitor_engine_SRCS
   systemmonitor.cpp
)

kde4_add_plugin(plasma_engine_systemmonitor ${systemmonitor_engine_SRCS})
target_link_libraries(plasma_engine_systemmonitor KDE4::plasma KDE4::kdecore ${QT_QTNETWORK_LIBRARY} ksgrd)

install(TARGETS plasma_engine_systemmonitor DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-systemmonitor.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )




