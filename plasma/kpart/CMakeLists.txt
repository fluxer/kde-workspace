
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

target_link_libraries(plasma-kpart ${QT_QTSCRIPT_LIBRARY} ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} ${X11_LIBRARIES} ${KDE4_KFILE_LIBS} ${KDE4_KPARTS_LIBS} )

install(TARGETS plasma-kpart DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-kpart.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

