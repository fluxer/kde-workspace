set(soliddevice_engine_SRCS
    soliddeviceengine.cpp
    devicesignalmapper.cpp
    devicesignalmapmanager.cpp
    soliddeviceservice.cpp
    soliddevicejob.cpp
)

kde4_add_plugin(plasma_engine_soliddevice ${soliddevice_engine_SRCS})
target_link_libraries(plasma_engine_soliddevice
    KDE4::kdecore
    KDE4::solid
    ${QT_QTNETWORK_LIBRARY}
    KDE4::plasma
    KDE4::kio
)

install(TARGETS plasma_engine_soliddevice DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-soliddevice.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
