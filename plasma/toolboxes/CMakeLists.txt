project(plasma-toolbox)

#Desktop toolbox
set(desktoptoolbox_SRCS internaltoolbox.cpp desktoptoolbox.cpp)


kde4_add_plugin(plasma_toolbox_desktoptoolbox ${desktoptoolbox_SRCS})
target_link_libraries(plasma_toolbox_desktoptoolbox KDE4::plasma KDE4::kdeui kworkspace)

install(TARGETS plasma_toolbox_desktoptoolbox
        DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES plasma-toolbox-desktoptoolbox.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR})


set(desktoptoolbox_SRCS internaltoolbox.cpp desktoptoolbox.cpp)


#Panel toolbox
set(paneltoolbox_SRCS internaltoolbox.cpp paneltoolbox.cpp)

kde4_add_plugin(plasma_toolbox_paneltoolbox ${paneltoolbox_SRCS})
target_link_libraries(plasma_toolbox_paneltoolbox KDE4::plasma KDE4::kdeui)

install(TARGETS plasma_toolbox_paneltoolbox
        DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES plasma-toolbox-paneltoolbox.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
