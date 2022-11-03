
add_subdirectory(module)

include_directories(${X11_Xrandr_INCLUDE_PATH})

########### next target ###############


set(randrinternal_PART_SRCS 
    ktimerdialog.cpp
    randr.cpp 
    randrdisplay.cpp
    collapsiblewidget.cpp
    layoutmanager.cpp 
    outputconfig.cpp
    outputgraphicsitem.cpp
    randrconfig.cpp
    randrcrtc.cpp 
    randrmode.cpp
    randroutput.cpp
    randrscreen.cpp)

set(kcm_randr_PART_SRCS 
    ${randrinternal_PART_SRCS}
    krandrmodule.cpp 
    randrconfigbase.ui
    outputconfigbase.ui
)

kde4_add_plugin(kcm_randr ${kcm_randr_PART_SRCS})

target_link_libraries(kcm_randr
    ${KDE4_KDEUI_LIBS}
    ${X11_Xrandr_LIB}
    ${X11_LIBRARIES}
)

install(
    TARGETS kcm_randr
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### next target ###############

set(krandrtray_SRCS
    ${randrinternal_PART_SRCS}
    main.cpp
    krandrtray.cpp
    krandrapp.cpp
    krandrpassivepopup.cpp
)

add_executable(krandrtray ${krandrtray_SRCS})

target_link_libraries(krandrtray
    ${KDE4_KCMUTILS_LIBS}
    ${X11_Xrandr_LIB}
    ${X11_LIBRARIES}
)

install(
    TARGETS krandrtray ${INSTALL_TARGETS_DEFAULT_ARGS}
)

########### next target ###############

install(
    FILES randr.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    PROGRAMS krandrtray.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
