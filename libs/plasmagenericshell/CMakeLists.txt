include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    # for kworkspace_export.h
    ${CMAKE_BINARY_DIR}/libs/kworkspace
)

set(plasmagenericshell_LIB_SRCS
    backgrounddialog.cpp
    mouseinputbutton.cpp
    mouseplugins.cpp
    mousepluginwidget.cpp
    panelshadows.cpp
    toolbutton.cpp
    wallpaperpreview.cpp
    scripting/appinterface.cpp
    scripting/applet.cpp
    scripting/containment.cpp
    scripting/configgroup.cpp
    scripting/i18n.cpp
    scripting/rect.cpp
    scripting/scriptengine.cpp
    scripting/widget.cpp
    widgetsexplorer/widgetexplorer.cpp
    BackgroundDialog.ui
    MousePlugins.ui
)

add_library(plasmagenericshell SHARED ${plasmagenericshell_LIB_SRCS})

target_link_libraries(plasmagenericshell PUBLIC
    KDE4::plasma
    KDE4::kdeui
    KDE4::kfile
    KDE4::solid
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTUITOOLS_LIBRARY}
    kworkspace
)
target_link_libraries(plasmagenericshell PRIVATE ${X11_LIBRARIES})

set_target_properties(plasmagenericshell PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(plasmagenericshell)

install(
    TARGETS plasmagenericshell
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)
