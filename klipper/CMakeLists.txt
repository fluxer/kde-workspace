set(libklipper_common_SRCS
    klipper.cpp
    urlgrabber.cpp
    configdialog.cpp
    history.cpp
    historyitem.cpp
    historystringitem.cpp
    klipperpopup.cpp
    popupproxy.cpp
    historyimageitem.cpp
    historyurlitem.cpp
    actionstreewidget.cpp
    editactiondialog.cpp
    clipcommandprocess.cpp
    generalconfig.ui
    actionsconfig.ui
    editactiondialog.ui
)

kde4_add_kcfg_files(libklipper_common_SRCS klippersettings.kcfgc)

set(klipper_SRCS ${libklipper_common_SRCS} main.cpp tray.cpp)

add_executable(klipper ${klipper_SRCS})

target_link_libraries(klipper
    KDE4::kdeui
    KDE4::kfile
    ${QT_QTNETWORK_LIBRARY}
    ${X11_LIBRARIES}
)
if (X11_Xfixes_FOUND)
    target_link_libraries(klipper ${X11_Xfixes_LIB})
endif (X11_Xfixes_FOUND)

install(
    TARGETS klipper
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    PROGRAMS klipper.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
install(
    PROGRAMS klipper.desktop
    DESTINATION ${KDE4_AUTOSTART_INSTALL_DIR}
)
