add_subdirectory(componentservices)
add_subdirectory(windowmanagers)

########### next target ###############

set(kcm_componentchooser_SRCS
    componentchooser.cpp
    componentchooserfilemanager.cpp
    componentchooseremail.cpp
    kcm_componentchooser.cpp
    componentchooserterminal.cpp
    componentchooserwm.cpp
)

kde4_add_plugin(kcm_componentchooser ${kcm_componentchooser_SRCS})

target_link_libraries(kcm_componentchooser
    KDE4::kio
    KDE4::kemail
)

install(
    TARGETS kcm_componentchooser
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES componentchooser.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
