include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

########### next target ###############

set(kded_randrmonitor_PART_SRCS
    randrmonitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randrdisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randrscreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randroutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randrcrtc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randrmode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../randr.cpp
)

kde4_add_plugin(kded_randrmonitor ${kded_randrmonitor_PART_SRCS})

target_link_libraries(kded_randrmonitor
    KDE4::kdeui
    KDE4::solid
    ${X11_Xrandr_LIB}
    ${X11_LIBRARIES}
)

install(
    TARGETS kded_randrmonitor
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES randrmonitor.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)
