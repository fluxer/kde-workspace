set(hotplug_engine_SRCS
    hotplugengine.cpp
    hotplugservice.cpp
    hotplugjob.cpp
)

kde4_add_plugin(plasma_engine_hotplug ${hotplug_engine_SRCS})
target_link_libraries(plasma_engine_hotplug
    KDE4::plasma
    KDE4::kdecore
    KDE4::solid
    KDE4::kio
)

install(
    TARGETS plasma_engine_hotplug
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-dataengine-hotplug.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
