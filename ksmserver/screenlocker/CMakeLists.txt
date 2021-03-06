# for dbus_add_activation_service
include(MacroDBusAddActivationService)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1223)
add_subdirectory(greeter)

include_directories(${CMAKE_SOURCE_DIR}/libs/kworkspace)

set(screensaver_dbusXML dbus/org.freedesktop.ScreenSaver.xml)
set(kscreensaver_dbusXML dbus/org.kde.screensaver.xml)
set(ksmserver_xml  ${CMAKE_SOURCE_DIR}/ksmserver/org.kde.KSMServerInterface.xml)
set(powerdevilpolicyagent_xml ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.Solid.PowerManagement.PolicyAgent.xml)

set(ksld_SRCS
   ksldapp.cpp
   autologout.cpp
   interface.cpp
   lockwindow.cpp)
qt4_add_dbus_adaptor(ksld_SRCS ${screensaver_dbusXML} interface.h ScreenLocker::Interface)
qt4_add_dbus_adaptor(ksld_SRCS ${kscreensaver_dbusXML} interface.h ScreenLocker::Interface kscreensaveradaptor KScreenSaverAdaptor)
kde4_add_kcfg_files(ksld_SRCS kcfg/kscreensaversettings.kcfgc)
QT4_ADD_DBUS_INTERFACE(ksld_SRCS ${ksmserver_xml} ksmserver_interface)
QT4_ADD_DBUS_INTERFACE(ksld_SRCS ${powerdevilpolicyagent_xml} powerdevilpolicyagent)

add_library(screenlocker_static STATIC ${ksld_SRCS})

target_link_libraries(screenlocker_static kworkspace ${KDE4_KDEUI_LIBS} ${KDE4_KIDLETIME_LIBS} ${X11_LIBRARIES})

# Needed to compile on Arm target.
set_target_properties(screenlocker_static PROPERTIES COMPILE_FLAGS "-fPIC")

install(FILES kscreenlocker.notifyrc  DESTINATION ${KDE4_DATA_INSTALL_DIR}/ksmserver/ RENAME ksmserver.notifyrc)
