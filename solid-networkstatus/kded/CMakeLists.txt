########### next target ###############

set(kded_networkstatus_PART_SRCS
   networkstatus.cpp
   network.cpp
   systemstatusinterface.cpp
   wicdstatus.cpp
   wicdcustomtypes.cpp
)

macro_optional_find_package(NetworkManager)
set_package_properties(NetworkManager PROPERTIES DESCRIPTION "The NetworkManager headers"
                       URL "http://projects.gnome.org/NetworkManager"
                       TYPE OPTIONAL
                       PURPOSE "Needed for kded's networkstatus module"
                      )

if(NETWORKMANAGER_FOUND)
    if(${NETWORKMANAGER_VERSION} VERSION_EQUAL "0.7.0"
        OR ${NETWORKMANAGER_VERSION} VERSION_GREATER "0.7.0")
        MESSAGE(STATUS "Adding in-process NetworkManager service-wart to kded module")
        set(kded_networkstatus_PART_SRCS ${kded_networkstatus_PART_SRCS} networkmanagerstatus.cpp)
        include_directories(${NETWORKMANAGER_INCLUDE_DIRS} ${NM-GLIB_INCLUDE_DIRS})
        add_definitions(-DNM_BACKEND_ENABLED -DQT_NO_KEYWORDS)
    endif()
endif(NETWORKMANAGER_FOUND)

qt4_add_dbus_adaptor(kded_networkstatus_PART_SRCS ../org.kde.Solid.Networking.Service.xml
        networkstatus.h NetworkStatusModule)

# we need one of the interfaces installed by kdelibs:
qt4_add_dbus_adaptor(kded_networkstatus_PART_SRCS
        ${KDE4_DBUS_INTERFACES_DIR}/org.kde.Solid.Networking.Client.xml
        networkstatus.h NetworkStatusModule)


kde4_add_plugin(kded_networkstatus ${kded_networkstatus_PART_SRCS})

target_link_libraries(kded_networkstatus ${KDE4_KDECORE_LIBS})

install(TARGETS kded_networkstatus DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES networkstatus.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)

