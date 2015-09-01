PROJECT (solid-actions)

ADD_SUBDIRECTORY(device-actions)

########### next target ###############

SET(kcm_solid_actions_srcs
    PredicateItem.cpp
    PredicateModel.cpp
    ActionItem.cpp
    ActionModel.cpp
    ActionEditor.cpp
    SolidActionData.cpp
    SolidActions.cpp
    SolidActions.ui
    AddAction.ui
    ActionEditor.ui
)

SET(solid_action_desktop_gen_srcs
    DesktopFileGenerator.cpp
    SolidActionData.cpp )

KDE4_ADD_PLUGIN(kcm_solid_actions ${kcm_solid_actions_srcs})
add_executable(solid-action-desktop-gen ${solid_action_desktop_gen_srcs})

TARGET_LINK_LIBRARIES(kcm_solid_actions ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} )
TARGET_LINK_LIBRARIES(solid-action-desktop-gen ${KDE4_SOLID_LIBS} ${KDE4_KIO_LIBS} )

########### install files ###############

INSTALL( TARGETS kcm_solid_actions DESTINATION ${PLUGIN_INSTALL_DIR} )
INSTALL( TARGETS solid-action-desktop-gen ${INSTALL_TARGETS_DEFAULT_ARGS} )
INSTALL( FILES solid-actions.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
INSTALL( FILES solid-action-template.desktop DESTINATION ${DATA_INSTALL_DIR}/kcmsolidactions )
INSTALL( FILES solid-device-type.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

