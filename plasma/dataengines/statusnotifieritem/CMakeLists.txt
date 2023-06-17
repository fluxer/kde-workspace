project(plasma-statusnotifieritem_engine)

include_directories(${CMAKE_SOURCE_DIR}/statusnotifierwatcher)

# We add our source code here
set(statusnotifieritem_engine_SRCS
    statusnotifieritem_engine.cpp
    statusnotifieritemsource.cpp
    statusnotifieritemservice.cpp
    statusnotifieritemjob.cpp
    systemtraytypes.cpp
)

set(statusnotifierwatcher_xml ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.StatusNotifierWatcher.xml)
QT4_ADD_DBUS_INTERFACE(statusnotifieritem_engine_SRCS ${statusnotifierwatcher_xml} statusnotifierwatcher_interface)

set(statusnotifieritem_xml ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.StatusNotifierItem.xml)

set_source_files_properties(${statusnotifieritem_xml} PROPERTIES
   NO_NAMESPACE false
   INCLUDE "systemtraytypes.h"
   CLASSNAME OrgKdeStatusNotifierItem
)
QT4_ADD_DBUS_INTERFACE(statusnotifieritem_engine_SRCS ${statusnotifieritem_xml} statusnotifieritem_interface)

kde4_add_plugin(plasma_engine_statusnotifieritem ${statusnotifieritem_engine_SRCS})
target_link_libraries(plasma_engine_statusnotifieritem KDE4::kdeui KDE4::plasma)

install(TARGETS plasma_engine_statusnotifieritem DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma_engine_statusnotifieritem.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
