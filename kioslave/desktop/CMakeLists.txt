if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

########### next target ###############

set(kio_desktop_PART_SRCS kio_desktop.cpp)

kde4_add_plugin(kio_desktop  ${kio_desktop_PART_SRCS})

target_link_libraries(kio_desktop  ${KDE4_KIO_LIBS})

install(TARGETS kio_desktop DESTINATION  ${KDE4_PLUGIN_INSTALL_DIR} )

########### next target ###############

set(kded_desktopnotifier_PART_SRCS  desktopnotifier.cpp )

kde4_add_plugin(kded_desktopnotifier  ${kded_desktopnotifier_PART_SRCS})

target_link_libraries(kded_desktopnotifier  ${KDE4_KIO_LIBS})

install(TARGETS kded_desktopnotifier  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES desktopnotifier.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR}/kded )
install( FILES desktop.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

install(FILES directory.desktop directory.trash DESTINATION ${KDE4_DATA_INSTALL_DIR}/kio_desktop)
install(FILES Home.desktop DESTINATION ${KDE4_DATA_INSTALL_DIR}/kio_desktop/DesktopLinks)

