add_subdirectory(componentservices)
add_subdirectory(windowmanagers)

########### next target ###############

set(kcm_componentchooser_SRCS
    componentchooser.cpp
    componentchooserbrowser.cpp
    componentchooserfilemanager.cpp
    componentchooseremail.cpp
    kcm_componentchooser.cpp
)

set(kcm_componentchooser_SRCS
    ${kcm_componentchooser_SRCS}
    componentchooserterminal.cpp
    ktimerdialog.cpp
)

if(Q_WS_X11)
    set(kcm_componentchooser_SRCS
        ${kcm_componentchooser_SRCS}
        componentchooserwm.cpp
    )
endif()

kde4_add_plugin(kcm_componentchooser ${kcm_componentchooser_SRCS})

target_link_libraries(kcm_componentchooser ${KDE4_KIO_LIBS})

install(
    TARGETS kcm_componentchooser
    DESTINATION ${PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES componentchooser.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
