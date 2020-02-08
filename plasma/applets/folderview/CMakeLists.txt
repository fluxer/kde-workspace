project(plasma-folderview)

include_directories(${LIBKONQ_INCLUDE_DIR})

if(Q_WS_X11)
  find_package(X11 REQUIRED)
endif(Q_WS_X11)

set(folderview_SRCS
    folderview.cpp
    abstractitemview.cpp
    listview.cpp
    dialog.cpp
    dialogshadows.cpp
    label.cpp
    iconview.cpp
    popupview.cpp
    iconwidget.cpp
    dirlister.cpp
    proxymodel.cpp
    folderviewadapter.cpp
    previewpluginsmodel.cpp
    tooltipwidget.cpp
    style.cpp
    itemeditor.cpp
    animator.cpp
    actionoverlay.cpp
    asyncfiletester.cpp
    folderviewFilterConfig.ui
    folderviewDisplayConfig.ui
    folderviewLocationConfig.ui
    folderviewPreviewConfig.ui
)

kde4_add_plugin(plasma_applet_folderview ${folderview_SRCS})

target_link_libraries(plasma_applet_folderview ${KDE4_PLASMA_LIBS} ${LIBKONQ_LIBRARY}
                      ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_SOLID_LIBS} ${X11_LIBRARIES})

install(TARGETS plasma_applet_folderview DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-folderview.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

