project(plasma-dataengine-mpris2)

set(mpris2_engine_SRCS
    mpris2engine.cpp
    multiplexer.cpp
    multiplexedservice.cpp
    playercontrol.cpp
    playeractionjob.cpp
    playercontainer.cpp
)

set_source_files_properties(
   org.freedesktop.DBus.Properties.xml
   org.mpris.MediaPlayer2.Player.xml
   org.mpris.MediaPlayer2.xml
   PROPERTIES
   NO_NAMESPACE ON)
qt4_add_dbus_interface(mpris2_engine_SRCS org.freedesktop.DBus.Properties.xml dbusproperties)
qt4_add_dbus_interface(mpris2_engine_SRCS org.mpris.MediaPlayer2.Player.xml mprisplayer)
qt4_add_dbus_interface(mpris2_engine_SRCS org.mpris.MediaPlayer2.xml mprisroot)

kde4_add_plugin(plasma_engine_mpris2 ${mpris2_engine_SRCS})
target_link_libraries(plasma_engine_mpris2
   KDE4::plasma
)

install(TARGETS plasma_engine_mpris2 DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-mpris2.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

