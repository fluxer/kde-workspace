set(notifications_engine_SRCS
    notificationsengine.cpp
    notificationservice.cpp
    notificationaction.cpp
)

qt4_add_dbus_adaptor( notifications_engine_SRCS org.freedesktop.Notifications.xml notificationsengine.h  NotificationsEngine )

kde4_add_plugin(plasma_engine_notifications ${notifications_engine_SRCS})

target_link_libraries(plasma_engine_notifications ${KDE4_PLASMA_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KNOTIFYCONFIG_LIBRARY})

install(TARGETS plasma_engine_notifications DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-notifications.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )
install(FILES notifications.operations DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/services)
