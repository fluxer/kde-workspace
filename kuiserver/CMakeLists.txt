if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

include_directories( ${KDE4_KIO_INCLUDES} )

# for dbus_add_activation_service
include(MacroDBusAddActivationService)

set(kuiserver_SRCS
    main.cpp
    uiserver.cpp
    jobview.cpp
    progresslistmodel.cpp
    progresslistdelegate.cpp
    requestviewcallwatcher.cpp
    configdialog.ui
)

set(kuiserver_KDEINIT_PRIVATE
    progresslistdelegate_p.h
    uiserver_p.h
)

#<DBus Stuff>

#<JobView>
  set(jobview_xml ${KDE4_DBUS_INTERFACES_DIR}/org.kde.JobViewV2.xml)

  qt4_add_dbus_adaptor(kuiserver_SRCS ${jobview_xml} jobview.h JobView jobviewadaptor )

  qt4_add_dbus_interface(kuiserver_SRCS ${jobview_xml} jobview_interface )
#</JobView>


#<JobViewServer>
  set(jobviewserver_xml ${KDE4_DBUS_INTERFACES_DIR}/org.kde.JobViewServer.xml)

  qt4_add_dbus_adaptor(kuiserver_SRCS ${jobviewserver_xml} progresslistmodel.h ProgressListModel jobviewserveradaptor )

  qt4_add_dbus_interface(kuiserver_SRCS ${jobviewserver_xml} jobviewserver_interface )
#</JobViewServer>

qt4_add_dbus_adaptor(kuiserver_SRCS org.kde.kuiserver.xml progresslistmodel.h ProgressListModel kuiserveradaptor )


dbus_add_activation_service(org.kde.kuiserver.service.in)

#</DBus Stuff>


kde4_add_kcfg_files(kuiserver_SRCS kuiserversettings.kcfgc)

add_executable( kuiserver ${kuiserver_SRCS})

target_link_libraries(kuiserver  ${KDE4_KIO_LIBS} )

install(TARGETS kuiserver ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES kuiserver.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
