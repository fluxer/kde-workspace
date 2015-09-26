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

macro_optional_find_package(Prison QUIET CONFIG)
set_package_properties(Prison PROPERTIES
    DESCRIPTION "Prison library"
    URL "http://projects.kde.org/prison"
    TYPE OPTIONAL
    PURPOSE "Needed to create mobile barcodes from clipboard data"
)

if (PRISON_FOUND)
    add_definitions(-DHAVE_PRISON)
    include_directories(${PRISON_INCLUDE_DIR})
endif (PRISON_FOUND)

kde4_add_kcfg_files(libklipper_common_SRCS klippersettings.kcfgc)

set(klipper_SRCS ${libklipper_common_SRCS} main.cpp tray.cpp)

add_executable(klipper ${klipper_SRCS})

target_link_libraries(klipper ${KDE4_KDEUI_LIBS} ${X11_LIBRARIES} ${ZLIB_LIBRARY})
if (X11_Xfixes_FOUND)
  target_link_libraries(klipper ${X11_Xfixes_LIB})
endif (X11_Xfixes_FOUND)
if (PRISON_FOUND)
  target_link_libraries(klipper ${PRISON_LIBRARIES})
endif (PRISON_FOUND)

install(TARGETS klipper ${INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS klipper.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(PROGRAMS klipper.desktop DESTINATION ${AUTOSTART_INSTALL_DIR})
