project(plasma-systemtray)

set(data_install_dir "plasma/packages/org.kde.systemtray")

cmake_policy(SET CMP0005 NEW)
add_definitions(-DSYSTEMTRAY_DATA_INSTALL_DIR="${data_install_dir}")

set(systemtray_SRCS
    core/manager.cpp
    core/protocol.cpp
    core/task.cpp

    protocols/fdo/fdoprotocol.cpp
    protocols/fdo/fdotask.cpp
    #FIXME: we should find another way to achieve that
    #protocols/fdo/fdonotification.cpp
    protocols/fdo/fdographicswidget.cpp
    protocols/fdo/fdoselectionmanager.cpp
    protocols/fdo/x11embedcontainer.cpp
    protocols/fdo/x11embeddelegate.cpp
    protocols/fdo/x11embedpainter.cpp

    protocols/plasmoid/plasmoidtaskprotocol.cpp
    protocols/plasmoid/plasmoidtask.cpp

    protocols/dbussystemtray/dbussystemtraytask.cpp
    protocols/dbussystemtray/dbussystemtrayprotocol.cpp

    ui/applet.cpp
    ui/widgetitem.cpp
    ui/mouseredirectarea.cpp
)

kde4_add_plugin(plasma_applet_systemtray ${systemtray_SRCS})
include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(plasma_applet_systemtray
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    ${X11_Xfixes_LIB}
)

if(X11_Xdamage_FOUND)
    target_link_libraries(plasma_applet_systemtray ${X11_Xdamage_LIB})
endif()
if(X11_Xcomposite_FOUND)
    target_link_libraries(plasma_applet_systemtray ${X11_Xcomposite_LIB})
endif()

install(TARGETS plasma_applet_systemtray DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(DIRECTORY "package/" DESTINATION "${KDE4_DATA_INSTALL_DIR}/${data_install_dir}")
install(FILES plasma-applet-systemtray.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
