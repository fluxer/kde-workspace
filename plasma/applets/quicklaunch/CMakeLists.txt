project(plasma-quicklaunch)

set(quicklaunch_SRCS
    icongridlayout.cpp
    launcher.cpp
    launcherdata.cpp
    launchergrid.cpp
    popup.cpp
    popuplauncherlist.cpp
    quicklaunch.cpp
    dropmarker.cpp
    quicklaunchConfig.ui
)

kde4_add_plugin(plasma_applet_quicklaunch ${quicklaunch_SRCS})

target_link_libraries(plasma_applet_quicklaunch
  KDE4::plasma
  KDE4::kdeui
  KDE4::kio)

install(TARGETS plasma_applet_quicklaunch DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-quicklaunch.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
