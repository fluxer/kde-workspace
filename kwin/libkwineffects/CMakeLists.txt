########### next target ###############

###  effects lib  ###
set(kwin_EFFECTSLIB_SRCS
    kwinglobals.cpp
    kwineffects.cpp
    anidata.cpp
    kwinanimationeffect.cpp
    kwinxrenderutils.cpp
)

set(kwineffects_QT_LIBS
    ${QT_QTGUI_LIBRARY}
)

set(kwineffects_KDE_LIBS
    ${KDE4_KDEUI_LIBS}
)

set(kwineffects_XLIB_LIBS
    ${X11_X11_LIB}
    ${X11_Xext_LIB}
    ${X11_Xrandr_LIB}
    ${X11_Xdamage_LIB}
)

set(kwineffects_XCB_LIBS
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
    ${XCB_XFIXES_LIBRARIES}
    ${XCB_RENDER_LIBRARIES}
)

add_library(kwineffects SHARED ${kwin_EFFECTSLIB_SRCS})
target_link_libraries(kwineffects PRIVATE
    ${kwineffects_QT_LIBS}
    ${kwineffects_KDE_LIBS}
    ${kwineffects_XLIB_LIBS}
    ${kwineffects_XCB_LIBS}
)
set_target_properties(kwineffects PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS kwineffects EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES
    kwinglobals.h
    kwineffects.h
    kwinanimationeffect.h
    kwinxrenderutils.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR} COMPONENT Devel)
