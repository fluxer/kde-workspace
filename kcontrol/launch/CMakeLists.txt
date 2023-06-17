


########### next target ###############

set(kcm_launch_PART_SRCS kcmlaunch.cpp )

set(kdesktop_xml  ${CMAKE_SOURCE_DIR}/krunner/dbus/org.kde.krunner.App.xml)
QT4_ADD_DBUS_INTERFACE( kcm_launch_PART_SRCS ${kdesktop_xml} krunner_interface )
set(kwin_xml ${CMAKE_SOURCE_DIR}/kwin/org.kde.KWin.xml)
set_source_files_properties(${kwin_xml} PROPERTIES INCLUDE "interface_util.h")
QT4_ADD_DBUS_INTERFACE( kcm_launch_PART_SRCS ${kwin_xml} kwin_interface )



kde4_add_plugin(kcm_launch ${kcm_launch_PART_SRCS})


target_link_libraries(kcm_launch  KDE4::kdeui ${QT_QTGUI_LIBRARY})

install(TARGETS kcm_launch  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcmlaunch.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

