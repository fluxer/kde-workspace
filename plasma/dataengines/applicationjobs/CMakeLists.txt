project(kuiserver_engine)

#include (KDE4Defaults)

set(kuiserver_engine_SRCS
    kuiserverengine.cpp
    jobcontrol.cpp
    jobaction.cpp
)

qt4_add_dbus_adaptor(kuiserver_engine_SRCS
    ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.JobView.xml
    kuiserverengine.h
    JobView
    jobviewadaptor
)
qt4_add_dbus_adaptor(kuiserver_engine_SRCS
    ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.JobViewServer.xml
    kuiserverengine.h
    KuiserverEngine
    jobviewserveradaptor
)

kde4_add_plugin(plasma_engine_applicationjobs ${kuiserver_engine_SRCS})
target_link_libraries(plasma_engine_applicationjobs
    KDE4::kdeui
    KDE4::kio
    KDE4::plasma
)

install(
    TARGETS plasma_engine_applicationjobs
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES plasma-dataengine-applicationjobs.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
