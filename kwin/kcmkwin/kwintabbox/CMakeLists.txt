include_directories( ${CMAKE_SOURCE_DIR}/kwin/tabbox )

########### next target ###############

ADD_DEFINITIONS(-DTABBOX_KCM)

set(kcm_kwintabbox_PART_SRCS
    main.cpp
    layoutpreview.cpp
    thumbnailitem.cpp
    ${CMAKE_SOURCE_DIR}/kwin/tabbox/clientmodel.cpp
    ${CMAKE_SOURCE_DIR}/kwin/tabbox/declarative.cpp
    ${CMAKE_SOURCE_DIR}/kwin/tabbox/desktopmodel.cpp
    ${CMAKE_SOURCE_DIR}/kwin/tabbox/tabboxconfig.cpp
    ${CMAKE_SOURCE_DIR}/kwin/tabbox/tabboxhandler.cpp
    main.ui
)

kde4_add_plugin(kcm_kwintabbox ${kcm_kwintabbox_PART_SRCS})

target_link_libraries(kcm_kwintabbox
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${X11_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${KDE4_KDECLARATIVE_LIBS}
)

install(TARGETS kcm_kwintabbox  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############
install( FILES kwintabbox.desktop DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES qml/main.qml DESTINATION ${DATA_INSTALL_DIR}/kwin/kcm_kwintabbox)
install( FILES thumbnails/konqueror.png thumbnails/kmail.png thumbnails/systemsettings.png thumbnails/dolphin.png DESTINATION ${DATA_INSTALL_DIR}/kwin/kcm_kwintabbox)
install( FILES kwinswitcher.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
