set(kwin_effect_OWN_LIBS
    kwineffects
)

set(kwin_effect_KDE_LIBS
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECLARATIVE_LIBS}
)

set(kwin_effect_QT_LIBS
    ${QT_QTDECLARATIVE_LIBRARY}
)

set(kwin_effect_XLIB_LIBS
    ${X11_X11_LIB}
    ${X11_Xcursor_LIB}
)

set(kwin_effect_XCB_LIBS
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
    ${XCB_IMAGE_LIBRARIES}
    ${XCB_XFIXES_LIBRARIES}
)

if(KWIN_BUILD_COMPOSITE )
    set(kwin_effect_XCB_LIBS ${kwin_effect_XCB_LIBS} ${XCB_RENDER_LIBRARIES})
endif()

macro(KWIN4_ADD_EFFECT_BACKEND name)
    kde4_add_plugin(${name} ${ARGN})
    target_link_libraries(${name}
        ${kwin_effect_OWN_LIBS} ${kwin_effect_KDE_LIBS}
        ${kwin_effect_QT_LIBS} ${kwin_effect_XLIB_LIBS}
        ${kwin_effect_XCB_LIBS}
    )
endmacro()

# Adds effect plugin with given name. Sources are given after the name
macro( KWIN4_ADD_EFFECT name )
    KWIN4_ADD_EFFECT_BACKEND(kwin4_effect_${name} ${ARGN})
    install( TARGETS kwin4_effect_${name} DESTINATION ${PLUGIN_INSTALL_DIR})
endmacro()

macro( KWIN4_ADD_EFFECT_CONFIG name )
    kde4_add_plugin(kcm_kwin4_effect_${name} ${ARGN})
    target_link_libraries(kcm_kwin4_effect_${name} kwineffects ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS})
    install(TARGETS kcm_kwin4_effect_${name} DESTINATION ${PLUGIN_INSTALL_DIR})
endmacro()

# Install the KWin/Effect service type
install(FILES kwineffect.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

# Create initial variables
set(kwin4_effect_builtins_sources)
set(kwin4_effect_builtins_config_sources configs_builtins.cpp)
set(kwin4_effect_include_directories)

###############################################################################
# Built-in effects go here

# Common effects
include( kscreen/CMakeLists.txt )
include( presentwindows/CMakeLists.txt )
include( screenedge/CMakeLists.txt )
include( slidingpopups/CMakeLists.txt )
include( taskbarthumbnail/CMakeLists.txt )

# Common effects only relevant to desktop
include( dashboard/CMakeLists.txt )
include( diminactive/CMakeLists.txt )
include( dimscreen/CMakeLists.txt )
include( highlightwindow/CMakeLists.txt )
include( minimizeanimation/CMakeLists.txt )
include( resize/CMakeLists.txt )
include( showfps/CMakeLists.txt )
include( showpaint/CMakeLists.txt )
include( slide/CMakeLists.txt )
include( slideback/CMakeLists.txt )
include( thumbnailaside/CMakeLists.txt )
include( windowgeometry/CMakeLists.txt )
include( zoom/CMakeLists.txt )
include( logout/CMakeLists.txt )
include( magnifier/CMakeLists.txt )
include( mousemark/CMakeLists.txt )
include( snaphelper/CMakeLists.txt )
include( trackmouse/CMakeLists.txt )

###############################################################################

# Add the builtins plugin
KWIN4_ADD_EFFECT( builtins ${kwin4_effect_builtins_sources} )
KWIN4_ADD_EFFECT_CONFIG( builtins ${kwin4_effect_builtins_config_sources} )
