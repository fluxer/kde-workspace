include_directories(${kephal_SOURCE_DIR}/kephal)

configure_file (../kephal/config-kephal.h.cmake
${CMAKE_CURRENT_BINARY_DIR}/config-kephal.h)

set(kephalshared_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/outputs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/screens.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/simplescreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/configurations.cpp
)
set(unused_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/simpleoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../kephal/desktopwidgetoutputs.cpp
)

set(kephal_service_SRCS
    ${kephalshared_SRCS}
    kephalservice.cpp
    backendoutputs.cpp
    externalconfiguration.cpp
    xml/xmltype.cpp
    xml/configurations_xml.cpp
    xml/xmlnodehandler.cpp
    backendconfigurations.cpp
    xmlconfiguration.cpp
    xmlconfigurations.cpp
)
set(unused_SRCS
    configurationscreens.cpp
    outputscreens.cpp
    dbus/dbusapi_screens.cpp
    dbus/dbusapi_outputs.cpp
    dbus/dbusapi_configurations.cpp
)

if(Q_WS_X11 AND XRANDR_1_2_FOUND)
    set(kephal_service_SRCS ${kephal_service_SRCS}
        xrandr12/randr.cpp
        xrandr12/randrscreen.cpp
        xrandr12/randroutput.cpp
        xrandr12/randrcrtc.cpp
        xrandr12/randrmode.cpp
        xrandr12/randrdisplay.cpp
        xrandroutputs.cpp
    )
endif(Q_WS_X11 AND XRANDR_1_2_FOUND)

#QT4_GENERATE_DBUS_INTERFACE(dbus/dbusapi_screens.h org.kde.Kephal.Screens.xml)
#QT4_GENERATE_DBUS_INTERFACE(dbus/dbusapi_outputs.h org.kde.Kephal.Outputs.xml)
#QT4_GENERATE_DBUS_INTERFACE(dbus/dbusapi_configurations.h org.kde.Kephal.Configurations.xml)

#QT4_ADD_DBUS_ADAPTOR (kephal_service_SRCS
#   ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Kephal.Screens.xml
#   dbus/dbusapi_screens.h DBusAPIScreens)
#QT4_ADD_DBUS_ADAPTOR (kephal_service_SRCS
#   ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Kephal.Outputs.xml
#   dbus/dbusapi_outputs.h DBusAPIOutputs)
#QT4_ADD_DBUS_ADAPTOR (kephal_service_SRCS
#   ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Kephal.Configurations.xml
#   dbus/dbusapi_configurations.h DBusAPIConfigurations)

set(kded_kephal_SRCS
    ${kephal_service_SRCS}
    kephald.cpp
)

set(service_libs ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTXML_LIBRARY})
if(Q_WS_X11 AND XRANDR_1_2_FOUND)
    set(service_libs ${service_libs} ${X11_Xrandr_LIB} ${X11_LIBRARIES})
endif()

KDE4_ADD_PLUGIN(kded_kephal ${kded_kephal_SRCS})
target_link_libraries(kded_kephal ${service_libs})

install(TARGETS kded_kephal DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES kephal.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)
