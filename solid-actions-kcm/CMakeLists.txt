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

INSTALL(
    TARGETS kcm_solid_actions
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
INSTALL(
    TARGETS solid-action-desktop-gen
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
INSTALL(
    FILES solid-actions.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
INSTALL(
    FILES solid-action-template.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kcmsolidactions
)
INSTALL(
    FILES solid-device-type.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)

