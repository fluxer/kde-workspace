
set(plasma_kpartscripting_SRCS
   scripting/rect.cpp
   scripting/scriptengine.cpp
   scripting/widget.cpp
)

set(plasma_kpart_SRCS
    ${plasma_kpartscripting_SRCS}
    plasmakpartcorona.cpp
    plasmakpartview.cpp
    plasmakpart.cpp
)

kde4_add_plugin(plasma-kpart ${plasma_kpart_SRCS})

target_link_libraries(plasma-kpart ${QT_QTSCRIPT_LIBRARY} KDE4::plasma KDE4::kio ${X11_LIBRARIES} KDE4::kfile KDE4::kparts )

install(TARGETS plasma-kpart DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-kpart.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

