
project(kstyle-oxygen)

add_subdirectory( config )
add_subdirectory( demo )

include_directories (animations debug transitions ${CMAKE_SOURCE_DIR}/libs/oxygen)

########### next target ###############

set(oxygen_PART_SRCS
    debug/oxygenwidgetexplorer.cpp
    oxygenblurhelper.cpp
    oxygenframeshadow.cpp
    oxygenmdiwindowshadow.cpp
    oxygenmnemonics.cpp
    oxygenpropertynames.cpp
    oxygenshadowhelper.cpp
    oxygensplitterproxy.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenwindowmanager.cpp
)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} oxygenstyle)

if ( X11_FOUND )
   target_link_libraries(oxygen ${X11_LIBRARIES})
endif( X11_FOUND )

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )

########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )
