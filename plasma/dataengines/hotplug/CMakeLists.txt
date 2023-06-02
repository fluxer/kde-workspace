set(hotplug_engine_SRCS
    hotplugengine.cpp
    hotplugservice.cpp
    hotplugjob.cpp
)

kde4_add_plugin(plasma_engine_hotplug ${hotplug_engine_SRCS})
target_link_libraries(plasma_engine_hotplug
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_SOLID_LIBS}
    ${KDE4_KIO_LIBS}
)

install(
    TARGETS plasma_engine_hotplug
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-dataengine-hotplug.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
