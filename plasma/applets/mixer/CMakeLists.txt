project(plasma-applet-mixer)

include_directories(${ALSA_INCLUDE_DIR})

set(mixer_SRCS
    mixer.cpp
)

kde4_add_plugin(plasma_applet_mixer ${mixer_SRCS})
target_link_libraries(plasma_applet_mixer
    KDE4::plasma
    ${ALSA_LIBRARIES}
)

install(
    TARGETS plasma_applet_mixer
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES plasma-applet-mixer.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
