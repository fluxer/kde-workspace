########### next target ###############

include_directories(${LIBUSB_INCLUDES})

set(kcm_usb_PART_SRCS kcmusb.cpp usbdevices.cpp )
kde4_add_plugin(kcm_usb ${kcm_usb_PART_SRCS})

target_link_libraries(kcm_usb
    KDE4::kdeui
    ${QT_QTGUI_LIBRARY}
)

target_link_libraries(kcm_usb ${LIBUSB_LIBRARIES})

install(
    TARGETS kcm_usb
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kcmusb.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
