include_directories(
	${CMAKE_SOURCE_DIR}/kcontrol/kfontinst/kcmfontinst/
	${CMAKE_SOURCE_DIR}/kcontrol/kfontinst/viewpart/
	${CMAKE_CURRENT_BINARY_DIR}
	)

set(kfontinst_bin_SRCS ${libkfontinstjobrunner_SRCS} ${libkfontinstdbusiface_SRCS} Installer.cpp )
# qt4_add_dbus_interface(kfontinst_bin_SRCS ../dbus/org.kde.fontinst.xml FontInstInterfaceBase)

set(kfontprint_bin_SRCS ${libkfontinstactionlabel_SRCS} Printer.cpp )
set(kfontview_bin_SRCS Viewer.cpp )

add_executable(kfontinst_bin ${kfontinst_bin_SRCS})
add_executable(kfontprint_bin ${kfontprint_bin_SRCS})
add_executable(kfontview_bin ${kfontview_bin_SRCS})

set_target_properties(kfontinst_bin PROPERTIES OUTPUT_NAME kfontinst)
set_target_properties(kfontprint_bin PROPERTIES OUTPUT_NAME kfontprint)
set_target_properties(kfontview_bin PROPERTIES OUTPUT_NAME kfontview)

target_link_libraries(kfontinst_bin ${KDE4_KIO_LIBS} ${KDE4_KDESU_LIBS} ${X11_X11_LIB} kfontinst)
target_link_libraries(kfontprint_bin ${X11_X11_LIB} ${KDE4_KDEUI_LIBS} ${FREETYPE_LIBRARIES} kfontinstui kfontinst)
target_link_libraries(kfontview_bin ${KDE4_KPARTS_LIBS} kfontinstui kfontinst )

install(TARGETS kfontinst_bin ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS kfontprint_bin DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR} )
install(TARGETS kfontview_bin ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES kfontviewui.rc DESTINATION ${KDE4_DATA_INSTALL_DIR}/kfontview )
install(PROGRAMS kfontview.desktop DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR} )
install(FILES installfont.desktop DESTINATION
${KDE4_SERVICES_INSTALL_DIR}/ServiceMenus )

kde4_install_icons( ${KDE4_ICON_INSTALL_DIR} )
