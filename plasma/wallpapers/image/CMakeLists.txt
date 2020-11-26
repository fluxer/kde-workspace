project(plasma-wallpaper-image)

set(image_SRCS
    image.cpp
    backgrounddelegate.cpp
    backgroundlistmodel.cpp
    removebuttonmanager.cpp
    removebutton.cpp
    itemsview.cpp
)

set(ksmserver_xml ${CMAKE_SOURCE_DIR}/ksmserver/org.kde.KSMServerInterface.xml)
qt4_add_dbus_interface(image_SRCS ${ksmserver_xml} ksmserver_interface)

kde4_add_plugin(plasma_wallpaper_image ${image_SRCS})
target_link_libraries(plasma_wallpaper_image ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS})

install(TARGETS plasma_wallpaper_image DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-wallpaper-image.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
