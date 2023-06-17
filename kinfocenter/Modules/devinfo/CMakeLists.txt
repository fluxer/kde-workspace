project( devinfo )

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp)

kde4_add_plugin( devinfo ${devinfoSources} )
target_link_libraries( devinfo KDE4::kio KDE4::solid)

install( TARGETS devinfo DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )
install( FILES devinfo.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} )
