include_directories(
    # for kworkspace_export.h
    ${CMAKE_BINARY_DIR}/libs/kworkspace
)

########### next target ###############

set(kcm_screensaver_PART_SRCS
    scrnsave.cpp
    testwin.cpp
    saverconfig.cpp
)

set(kscreensaver_xml  ${CMAKE_SOURCE_DIR}/ksmserver/screenlocker/dbus/org.kde.screensaver.xml)
QT4_ADD_DBUS_INTERFACE( kcm_screensaver_PART_SRCS ${kscreensaver_xml} kscreensaver_interface )

kde4_add_plugin(kcm_screensaver ${kcm_screensaver_PART_SRCS})


target_link_libraries(kcm_screensaver ${KDE4_PLASMA_LIBS} ${X11_LIBRARIES} kworkspace)

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
  target_link_libraries(kcm_screensaver ${DL_LIBRARY})
endif(DL_LIBRARY)

install(TARGETS kcm_screensaver  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES screensaver.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )

