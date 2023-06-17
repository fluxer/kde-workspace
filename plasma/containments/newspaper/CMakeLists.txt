
set(newspaper_SRCS
    newspaper.cpp
    appletoverlay.cpp
    appletmovespacer.cpp
    applettitlebar.cpp
    appletscontainer.cpp
    appletsview.cpp
    dragcountdown.cpp
    )

kde4_add_plugin(plasma_containment_newspaper ${newspaper_SRCS})
target_link_libraries(plasma_containment_newspaper KDE4::plasma KDE4::kio)

install(TARGETS plasma_containment_newspaper DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containment-newspaper.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
