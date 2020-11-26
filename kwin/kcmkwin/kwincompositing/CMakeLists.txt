########### next target ###############

include_directories( ${CMAKE_SOURCE_DIR}/kwin )

set(kcm_kwincompositing_PART_SRCS
    main.cpp
    ktimerdialog.cpp
    main.ui
)

set(kwin_xml ${CMAKE_SOURCE_DIR}/kwin/org.kde.KWin.xml)
set_source_files_properties(${kwin_xml} PROPERTIES INCLUDE "interface_util.h")
QT4_ADD_DBUS_INTERFACE(kcm_kwincompositing_PART_SRCS ${kwin_xml} kwin_interface)
kde4_add_plugin(kcm_kwincompositing ${kcm_kwincompositing_PART_SRCS})
target_link_libraries(kcm_kwincompositing ${KDE4_KCMUTILS_LIBS} ${KDE4_KDEUI_LIBS} ${X11_LIBRARIES})
install(TARGETS kcm_kwincompositing  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kwincompositing.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )


