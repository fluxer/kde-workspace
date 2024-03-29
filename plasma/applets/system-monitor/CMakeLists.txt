project(plasma-system-monitor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} )

# lib

set(lib_SRCS
    monitoricon.cpp
    applet.cpp
    plotter.cpp
)

add_library(plasma_applet-system-monitor SHARED ${lib_SRCS})
target_link_libraries(plasma_applet-system-monitor
    KDE4::plasma
    KDE4::kdeui
)

set_target_properties(plasma_applet-system-monitor PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(plasma_applet-system-monitor BASE_NAME plasma_applet_system_monitor)

# HDD Space Monitor

set(hdd_SRCS
    hdd.cpp
    hdd-config.ui
)
kde4_add_plugin(plasma_applet_sm_hdd ${hdd_SRCS})
target_link_libraries(plasma_applet_sm_hdd KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# HDD Activity Monitor

set(hdd_activity_SRCS
    hdd_activity.cpp
    hdd_activity-config.ui
)
kde4_add_plugin(plasma_applet_sm_hdd_activity ${hdd_activity_SRCS})
target_link_libraries(plasma_applet_sm_hdd_activity KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Temperature Monitor

set(temparature_SRCS
    temperature.cpp
    temperature-offset-delegate.cpp
    temperature-config.ui
)
kde4_add_plugin(plasma_applet_sm_temperature ${temparature_SRCS})
target_link_libraries(plasma_applet_sm_temperature KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Network Monitor

set(net_SRCS
    net.cpp
    net-config.ui
)
kde4_add_plugin(plasma_applet_sm_net ${net_SRCS})
target_link_libraries(plasma_applet_sm_net KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# CPU Monitor

set(cpu_SRCS
    cpu.cpp
    cpu-config.ui
)
kde4_add_plugin(plasma_applet_sm_cpu ${cpu_SRCS})
target_link_libraries(plasma_applet_sm_cpu KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Harware Info

set(hwinfo_SRCS
    hwinfo.cpp
)
kde4_add_plugin(plasma_applet_sm_hwinfo ${hwinfo_SRCS})
target_link_libraries(plasma_applet_sm_hwinfo KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Ram Monitor

set(ram_SRCS
    ram.cpp
    ram-config.ui
)

kde4_add_plugin(plasma_applet_sm_ram ${ram_SRCS})
target_link_libraries(plasma_applet_sm_ram KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Main applet

set(system-monitor_SRCS
    system-monitor.cpp
    monitorbutton.cpp
)
kde4_add_plugin(plasma_applet_system-monitor ${system-monitor_SRCS})
target_link_libraries(plasma_applet_system-monitor KDE4::plasma KDE4::kdeui plasma_applet-system-monitor)

# Install
install(
    TARGETS plasma_applet-system-monitor
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)
install(
    TARGETS
    plasma_applet_sm_hdd_activity
    plasma_applet_sm_hdd
    plasma_applet_sm_temperature
    plasma_applet_sm_hwinfo
    plasma_applet_sm_net
    plasma_applet_sm_cpu
    plasma_applet_sm_ram
    plasma_applet_system-monitor
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
install(
    FILES
    plasma-applet-sm_hdd_activity.desktop
    plasma-applet-sm_hdd.desktop
    plasma-applet-sm_temperature.desktop
    plasma-applet-sm_hwinfo.desktop
    plasma-applet-sm_net.desktop
    plasma-applet-sm_cpu.desktop
    plasma-applet-sm_ram.desktop
    plasma-applet-system-monitor.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
install(
    FILES hdd_panel.svgz
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/desktoptheme/default/system-monitor/
)
