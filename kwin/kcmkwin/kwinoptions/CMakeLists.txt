########### next target ###############

set(kcm_kwinoptions_PART_SRCS
    windows.cpp
    mouse.cpp
    main.cpp
    actions.ui
    advanced.ui
    focus.ui
    mouse.ui
    moving.ui
)

kde4_add_plugin(kcm_kwinoptions ${kcm_kwinoptions_PART_SRCS})
target_link_libraries(kcm_kwinoptions KDE4::kdeui)

install(
    TARGETS kcm_kwinoptions
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES
    kwinoptions.desktop
    kwinactions.desktop
    kwinadvanced.desktop
    kwinfocus.desktop kwinmoving.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    FILES
    buttons/max1.png
    buttons/max2.png
    buttons/max3.png
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/kcm_kwinoptions/buttons
)

kde4_install_icons(${KDE4_ICON_INSTALL_DIR})
