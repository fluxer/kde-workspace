########### next target ###############

set(kcm_joystick_PART_SRCS 
    joystick.cpp
    joywidget.cpp
    poswidget.cpp
    joydevice.cpp
    caldialog.cpp
)

kde4_add_plugin(kcm_joystick ${kcm_joystick_PART_SRCS})

target_link_libraries(kcm_joystick
    KDE4::kdeui
    KDE4::kio
    ${QT_QTGUI_LIBRARY}
)

########### install files ###############

install(
    TARGETS kcm_joystick
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES joystick.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES joystick_new.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/solid/actions
)
