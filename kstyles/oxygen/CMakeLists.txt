
project(kstyle-oxygen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/debug
    ${CMAKE_SOURCE_DIR}/libs/oxygen
    ${CMAKE_BINARY_DIR}/libs/oxygen
)

add_subdirectory( config )
add_subdirectory( demo )

########### next target ###############

set(oxygen_PART_SRCS
    debug/oxygenwidgetexplorer.cpp
    oxygenframeshadow.cpp
    oxygenmdiwindowshadow.cpp
    oxygenmnemonics.cpp
    oxygenpropertynames.cpp
    oxygenshadowhelper.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenwindowmanager.cpp
)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} oxygenstyle)

if ( X11_FOUND )
   target_link_libraries(oxygen ${X11_LIBRARIES})
endif( X11_FOUND )

install(TARGETS oxygen  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}/plugins/styles/ )

########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kstyle/themes )
