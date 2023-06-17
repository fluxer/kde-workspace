include_directories(
    ${CMAKE_SOURCE_DIR}/libs/oxygen
    ${CMAKE_BINARY_DIR}/libs/oxygen
)

add_subdirectory( config )
add_subdirectory( demo )

########### next target ###############
set(kwin_oxygen_SRCS
     oxygenbutton.cpp
     oxygenclient.cpp
     oxygenclientgroupitemdata.cpp
     oxygendecohelper.cpp
     oxygenexceptionlist.cpp
     oxygenfactory.cpp
     oxygensizegrip.cpp
)

kde4_add_kcfg_files(kwin_oxygen_SRCS oxygenconfiguration.kcfgc)

kde4_add_plugin(kwin3_oxygen ${kwin_oxygen_SRCS})
target_link_libraries(kwin3_oxygen
    ${KDE4_KDEUI_LIBS}
    ${X11_X11_LIB}
    kdecorations
    oxygenstyle
)

install(
    TARGETS kwin3_oxygen
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############
install(
    FILES oxygenclient.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin
)
