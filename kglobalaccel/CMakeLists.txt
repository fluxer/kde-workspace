project(kglobalaccel)

###############################################################################
### KDED Global Accel Daemon

include_directories(${KDE4_KDEUI_INCLUDES})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config-kglobalaccel.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kglobalaccel.h
)

set(kglobalaccel_SRCS
    main.cpp
    kglobalacceld.cpp
    component.cpp
    globalshortcut.cpp
    globalshortcutsregistry.cpp
    globalshortcutcontext.cpp
)

if(Q_WS_X11)
    set(kglobalaccel_SRCS ${kglobalaccel_SRCS} kglobalaccel_x11.cpp)
endif()

add_executable( kglobalaccel ${kglobalaccel_SRCS})

target_link_libraries(kglobalaccel ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${X11_LIBRARIES})

# Install application and configuration
install( TARGETS kglobalaccel ${INSTALL_TARGETS_DEFAULT_ARGS} )
# install( FILES kglobalaccel.desktop DESTINATION ${AUTOSTART_INSTALL_DIR})
install( FILES kglobalaccel.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
# KNotify configuration
install( FILES kglobalaccel.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kglobalaccel)

