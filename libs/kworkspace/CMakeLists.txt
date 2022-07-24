set(kworkspace_LIB_SRCS
    kdisplaymanager.cpp
    kwindowlistmenu.cpp
    screenpreviewwidget.cpp
    kworkspace.cpp
)


set(ksmserver_xml ${CMAKE_SOURCE_DIR}/ksmserver/org.kde.KSMServerInterface.xml)
qt_add_dbus_interface(kworkspace_LIB_SRCS ${ksmserver_xml} ksmserver_interface)

set(kwin_xml ${CMAKE_SOURCE_DIR}/kwin/org.kde.KWin.xml)

set_source_files_properties(${kwin_xml} PROPERTIES INCLUDE "interface_util.h")

qt_add_dbus_interface(kworkspace_LIB_SRCS ${kwin_xml} kwin_interface)

add_library(kworkspace SHARED ${kworkspace_LIB_SRCS})

target_link_libraries(kworkspace PRIVATE
    ${X11_Xau_LIB}
    ${X11_SM_LIB}
    ${X11_ICE_LIB}
)

target_link_libraries(kworkspace PUBLIC
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
)

set_target_properties(kworkspace PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(kworkspace)

install(
    TARGETS kworkspace
    EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
)

install(
    FILES
    kdisplaymanager.h
    kwindowlistmenu.h
    kworkspace.h
    ${CMAKE_CURRENT_BINARY_DIR}/kworkspace_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/kworkspace
    COMPONENT Devel
)
