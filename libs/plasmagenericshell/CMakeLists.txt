include_directories(
   # for kworkspace_export.h
   ${CMAKE_BINARY_DIR}/libs/kworkspace
)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(appletbrowserdialog_SRCS
    widgetsexplorer/kcategorizeditemsviewmodels.cpp
    widgetsexplorer/plasmaappletitemmodel.cpp
    widgetsexplorer/widgetexplorer.cpp
    widgetsexplorer/openwidgetassistant.cpp
)

set(scripting_SRC
    scripting/appinterface.cpp
    scripting/applet.cpp
    scripting/containment.cpp
    scripting/configgroup.cpp
    scripting/i18n.cpp
    scripting/layouttemplatepackagestructure.cpp
    scripting/rect.cpp
    scripting/scriptengine.cpp
    scripting/widget.cpp
)

set(plasmagenericshell_LIB_SRCS
    backgrounddialog.cpp
    mouseinputbutton.cpp
    mouseplugins.cpp
    mousepluginwidget.cpp
    panelshadows.cpp
    toolbutton.cpp
    wallpaperpreview.cpp
    ${scripting_SRC}
    ${appletbrowserdialog_SRCS}
    BackgroundDialog.ui
    MousePlugins.ui
)

add_library(plasmagenericshell SHARED ${plasmagenericshell_LIB_SRCS})

target_link_libraries(plasmagenericshell PUBLIC
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTUITOOLS_LIBRARY}
    kworkspace
)

if(Q_WS_X11)
    target_link_libraries(plasmagenericshell PRIVATE ${X11_LIBRARIES})
endif(Q_WS_X11)

set_target_properties(plasmagenericshell PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(DIRECTORY widgetsexplorer/package/
        DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/packages/org.kde.desktop.widgetexplorer
        PATTERN "Messages.sh" EXCLUDE
)

install(TARGETS plasmagenericshell EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )
INSTALL(FILES plasma-layout-template.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})

