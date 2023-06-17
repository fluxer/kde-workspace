
set(powermanagement_engine_SRCS
    powermanagementengine.cpp
    powermanagementjob.cpp
    powermanagementservice.cpp
)

set(krunner_xml ${CMAKE_SOURCE_DIR}/krunner/dbus/org.kde.krunner.App.xml)
QT4_ADD_DBUS_INTERFACE(powermanagement_engine_SRCS ${krunner_xml} krunner_interface)

kde4_add_plugin(plasma_engine_powermanagement ${powermanagement_engine_SRCS})
target_link_libraries(plasma_engine_powermanagement KDE4::kdecore KDE4::solid KDE4::plasma KDE4::kidletime kworkspace)

install(TARGETS plasma_engine_powermanagement DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-powermanagement.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

