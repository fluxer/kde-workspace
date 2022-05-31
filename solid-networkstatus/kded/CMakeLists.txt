########### next target ###############

set(kded_networkstatus_PART_SRCS
   networkstatus.cpp
   network.cpp
   systemstatusinterface.cpp
   connmanstatus.cpp
   toolkitstatus.cpp
   networkmanagerstatus.cpp
)

qt4_add_dbus_adaptor(kded_networkstatus_PART_SRCS ../org.kde.Solid.Networking.Service.xml
        networkstatus.h NetworkStatusModule)

# we need one of the interfaces installed by kdelibs:
qt4_add_dbus_adaptor(kded_networkstatus_PART_SRCS
        ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.Solid.Networking.Client.xml
        networkstatus.h NetworkStatusModule)


kde4_add_plugin(kded_networkstatus ${kded_networkstatus_PART_SRCS})

target_link_libraries(kded_networkstatus ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS kded_networkstatus DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES networkstatus.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)

install(FILES networkstatus.notifyrc DESTINATION ${KDE4_DATA_INSTALL_DIR}/networkstatus)
