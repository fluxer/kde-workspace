include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    # for kworkspace_export.h
    ${CMAKE_BINARY_DIR}/libs/kworkspace
)

add_subdirectory(actions)

# Add bundled actions
set(powerdevil_bundled_actions_SRCS
    actions/bundled/suspendsession.cpp
    actions/bundled/brightnesscontrol.cpp
    actions/bundled/keyboardbrightnesscontrol.cpp
    actions/bundled/dimdisplay.cpp
    actions/bundled/runscript.cpp
    actions/bundled/handlebuttonevents.cpp
)

# target no.1 - powerdevil core library
set(powerdevilcore_SRCS
    powerdevilaction.cpp
    powerdevilactionpool.cpp
    powerdevilbackendinterface.cpp
    powerdevilcore.cpp
    powerdevilpolicyagent.cpp
    powerdevilprofilegenerator.cpp

    brightnessosdwidget.cpp
)

kde4_add_kcfg_files(powerdevilcore_SRCS ../PowerDevilSettings.kcfgc)

set(screensaver_xml "${CMAKE_SOURCE_DIR}/ksmserver/screenlocker/dbus/org.freedesktop.ScreenSaver.xml")
qt4_add_dbus_interface(powerdevilcore_SRCS ${screensaver_xml} screensaver_interface )

# Action DBus Adaptors
qt4_add_dbus_adaptor(powerdevilcore_SRCS actions/bundled/org.kde.Solid.PowerManagement.Actions.BrightnessControl.xml
                     actions/bundled/brightnesscontrol.h PowerDevil::BundledActions::BrightnessControl)
qt4_add_dbus_adaptor(powerdevilcore_SRCS actions/bundled/org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl.xml
                     actions/bundled/keyboardbrightnesscontrol.h PowerDevil::BundledActions::KeyboardBrightnessControl)
qt4_add_dbus_adaptor(powerdevilcore_SRCS actions/bundled/org.kde.Solid.PowerManagement.Actions.HandleButtonEvents.xml
                     actions/bundled/handlebuttonevents.h PowerDevil::BundledActions::HandleButtonEvents)
qt4_add_dbus_adaptor(powerdevilcore_SRCS actions/bundled/org.kde.Solid.PowerManagement.Actions.SuspendSession.xml
                     actions/bundled/suspendsession.h PowerDevil::BundledActions::SuspendSession)

add_library(powerdevilcore SHARED ${powerdevilcore_SRCS} ${powerdevil_bundled_actions_SRCS})

set_target_properties(powerdevilcore PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )

target_link_libraries(powerdevilcore
    ${KDE4_KDECORE_LIBS}
    ${KDE4_SOLID_LIBS}
    ${KDE4_KIDLETIME_LIBS}
    ${KDE4_PLASMA_LIBS}
    kworkspace
)

# target no.2 - powerdevil kded module
set(kded_powerdevil_SRCS
    kdedpowerdevil.cpp
    powerdevilbackendloader.cpp
    powerdevilfdoconnector.cpp
)

# DBus Adaptors
qt4_add_dbus_adaptor(kded_powerdevil_SRCS org.kde.Solid.PowerManagement.xml powerdevilcore.h PowerDevil::Core)
qt4_add_dbus_adaptor(kded_powerdevil_SRCS ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.kde.Solid.PowerManagement.PolicyAgent.xml
                                         powerdevilpolicyagent.h PowerDevil::PolicyAgent
                                         powermanagementpolicyagentadaptor PowerManagementPolicyAgentAdaptor)

qt4_add_dbus_adaptor(kded_powerdevil_SRCS ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.freedesktop.PowerManagement.xml powerdevilfdoconnector.h PowerDevil::FdoConnector powermanagementfdoadaptor PowerManagementFdoAdaptor)
qt4_add_dbus_adaptor(kded_powerdevil_SRCS ${KDE4_DBUS_INTERFACES_INSTALL_DIR}/org.freedesktop.PowerManagement.Inhibit.xml powerdevilfdoconnector.h PowerDevil::FdoConnector powermanagementinhibitadaptor PowerManagementInhibitAdaptor)


# Backends
include(BackendConfig.cmake)

kde4_add_plugin(kded_powerdevil ${kded_powerdevil_SRCS} ${POWERDEVIL_BACKEND_SRCS})

target_link_libraries(kded_powerdevil
    ${KDE4_KDECORE_LIBS}
    ${POWERDEVIL_BACKEND_LIBS}
    powerdevilcore
)

install(TARGETS kded_powerdevil DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(TARGETS powerdevilcore ${INSTALL_TARGETS_DEFAULT_ARGS})

# target no.3 - powerdevil ui library
set(powerdevilui_SRCS
    powerdevilactionconfig.cpp
)

add_library(powerdevilui SHARED ${powerdevilui_SRCS})
set_target_properties(powerdevilui PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

target_link_libraries(powerdevilui
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
)

install(TARGETS powerdevilui ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES powerdevil.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)
