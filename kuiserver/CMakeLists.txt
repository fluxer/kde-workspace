set(kuiserver_SRCS
    main.cpp
    jobview.cpp
    progresslistmodel.cpp
    progresslistdelegate.cpp
    requestviewcallwatcher.cpp
)

#<DBus Stuff>

#<JobView>
set(jobview_xml ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.JobView.xml)

qt4_add_dbus_adaptor(kuiserver_SRCS ${jobview_xml} jobview.h JobView jobviewadaptor )

qt4_add_dbus_interface(kuiserver_SRCS ${jobview_xml} jobview_interface )
#</JobView>

#<JobViewServer>
set(jobviewserver_xml ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.JobViewServer.xml)

qt4_add_dbus_adaptor(kuiserver_SRCS ${jobviewserver_xml} progresslistmodel.h ProgressListModel jobviewserveradaptor)

qt4_add_dbus_interface(kuiserver_SRCS ${jobviewserver_xml} jobviewserver_interface )
#</JobViewServer>

qt4_add_dbus_adaptor(kuiserver_SRCS org.kde.kuiserver.xml progresslistmodel.h ProgressListModel kuiserveradaptor)

kde4_add_dbus_service(org.kde.kuiserver.service.in)

#</DBus Stuff>


kde4_add_kcfg_files(kuiserver_SRCS kuiserversettings.kcfgc)

add_executable(kuiserver ${kuiserver_SRCS})

target_link_libraries(kuiserver KDE4::kio)

install(
    TARGETS kuiserver
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    FILES kuiserver.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
