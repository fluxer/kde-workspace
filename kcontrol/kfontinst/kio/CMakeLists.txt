# include_directories(${CMAKE_SOURCE_DIR}/kcontrol/fonts
#                     ${CMAKE_SOURCE_DIR}/kcontrol/kfontinst/kcmfontinst
#                     ${CMAKE_CURRENT_BINARY_DIR})

set(kio_fonts_PART_SRCS FontInstInterface.cpp KioFonts.cpp ${libkfontinstdbusiface_SRCS})
# qt4_add_dbus_interface(kio_fonts_PART_SRCS ../dbus/org.kde.fontinst.xml FontinstIface)
kde4_add_plugin(kio_fonts ${kio_fonts_PART_SRCS} ${KFI_FONTINST_AUTH_SRC} )
target_link_libraries(kio_fonts ${KDE4_KIO_LIBS}  ${QT_QTXML_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTCORE_LIBRARY} kfontinst )
install(TARGETS kio_fonts  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install( FILES fonts.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
kde4_install_icons( ${KDE4_ICON_INSTALL_DIR}  )
