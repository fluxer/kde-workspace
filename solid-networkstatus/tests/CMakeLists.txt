set(networkingservice_SRCS networkingservice.cpp networkingserviceview.ui)

qt_add_dbus_interface(networkingservice_SRCS ../org.kde.Solid.Networking.Service.xml serviceinterface)

add_executable(networkingservice ${networkingservice_SRCS})

target_link_libraries(networkingservice ${KDE4_KIO_LIBS})

