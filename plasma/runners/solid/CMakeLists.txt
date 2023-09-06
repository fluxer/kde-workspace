include_directories(
    ${CMAKE_SOURCE_DIR}/soliduiserver
)

set(krunner_solid_SRCS
    solidrunner.cpp
)

kde4_add_plugin(krunner_solid ${krunner_solid_SRCS})
target_link_libraries(krunner_solid
    KDE4::kdeui
    KDE4::plasma
    KDE4::solid
    KDE4::kio
)

install(
    TARGETS krunner_solid
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-runner-solid.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
