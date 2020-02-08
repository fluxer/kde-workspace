project(plasma-wallpaper-color)

set(color_SRCS
    color.cpp
    backgrounddelegate.cpp
    backgroundlistmodel.cpp
    itemsview.cpp
    config.ui
)

kde4_add_plugin(plasma_wallpaper_color ${color_SRCS})
target_link_libraries(plasma_wallpaper_color ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_wallpaper_color DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-wallpaper-color.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
