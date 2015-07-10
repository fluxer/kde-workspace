########### configure tests ###############
INCLUDE(CMakeDependentOption)

OPTION(KWIN_BUILD_DECORATIONS "Enable building of KWin decorations." ON)
OPTION(KWIN_BUILD_OXYGEN "Enable building of default decoration Oxygen" ON)
OPTION(KWIN_BUILD_KCMS "Enable building of KWin configuration modules." ON)
OPTION(KWIN_BUILD_TABBOX "Enable building of KWin Tabbox functionality" ON)
OPTION(KWIN_BUILD_SCREENEDGES "Enable building of KWin with screen edge support" ON)
OPTION(KWIN_BUILD_KAPPMENU "Enable building of KWin with application menu support" ON)
OPTION(KWIN_BUILD_XRENDER_COMPOSITING "Enable building of KWin with XRender Compositing support" ON)
OPTION(KWIN_BUILD_OPENGL_1_COMPOSITING "Enable support for OpenGL 1.x, automatically disabled when building for OpenGL ES 2.0" ON)

cmake_dependent_option(KWIN_BUILD_KAPPMENU "Build without appmenu support" ON "KWIN_BUILD_DECORATIONS" FALSE)

# KWIN_HAVE_XRENDER_COMPOSITING - whether XRender-based compositing support is available: may be disabled
if( KWIN_BUILD_XRENDER_COMPOSITING )
    set( KWIN_HAVE_XRENDER_COMPOSITING 1 )
endif()

if(OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
endif()

if(OPENGL_EGL_FOUND)
    include_directories(${OPENGLES_EGL_INCLUDE_DIR})
    set(KWIN_HAVE_EGL 1)
endif()

if(OPENGLES_FOUND)
  include_directories(${OPENGLES_INCLUDE_DIR})
endif()

include_directories(${XCB_INCLUDE_DIR})

# for things that are also used by kwin libraries
configure_file(libkwineffects/kwinconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkwineffects/kwinconfig.h )
# for kwin internal things
configure_file(config-kwin.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kwin.h )


########### global ###############

include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}/libkwineffects
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libkwineffects
    ${CMAKE_CURRENT_SOURCE_DIR}/libkdecorations
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    ${CMAKE_CURRENT_SOURCE_DIR}/tabbox
    )

add_subdirectory( libkdecorations )
add_subdirectory( libkwineffects )
add_subdirectory( killer )
if(KWIN_BUILD_KCMS)
    add_subdirectory( kcmkwin )
endif()

if( KWIN_BUILD_DECORATIONS )
    add_subdirectory( clients )
endif()
add_subdirectory( data )

add_subdirectory( effects )
add_subdirectory( tabbox )


########### next target ###############

set(kwin_SRCS
   workspace.cpp 
   dbusinterface.cpp
   client.cpp 
   client_machine.cpp
   cursor.cpp
   tabgroup.cpp
   focuschain.cpp
   netinfo.cpp
   placement.cpp 
   atoms.cpp 
   utils.cpp 
   layers.cpp 
   main.cpp 
   options.cpp 
   outline.cpp
   decorations.cpp
   events.cpp 
   killwindow.cpp 
   geometrytip.cpp 
   screens.cpp
   shadow.cpp
   sm.cpp 
   group.cpp 
   bridge.cpp 
   manage.cpp 
   overlaywindow.cpp
   activation.cpp 
   useractions.cpp 
   geometry.cpp 
   rules.cpp
   composite.cpp
   toplevel.cpp
   unmanaged.cpp
   scene.cpp
   scene_xrender.cpp
   scene_opengl.cpp
   glxbackend.cpp
   thumbnailitem.cpp
   lanczosfilter.cpp
   deleted.cpp
   effects.cpp
   compositingprefs.cpp
   paintredirector.cpp
   virtualdesktops.cpp
   xcbutils.cpp
   )

if(KWIN_BUILD_TABBOX)
    set(
        kwin_SRCS ${kwin_SRCS}
        tabbox/tabbox.cpp
        tabbox/clientmodel.cpp
        tabbox/declarative.cpp
        tabbox/desktopchain.cpp
        tabbox/desktopmodel.cpp
        tabbox/tabboxconfig.cpp
        tabbox/tabboxhandler.cpp
  )
endif()

if(KWIN_BUILD_SCREENEDGES)
    set(
        kwin_SRCS ${kwin_SRCS}
        screenedge.cpp
    )
endif()

if(KWIN_BUILD_KAPPMENU)
    set(
        kwin_SRCS ${kwin_SRCS}
        appmenu.cpp
    )
endif()

if(KWIN_HAVE_EGL)
    set(kwin_SRCS ${kwin_SRCS} eglonxbackend.cpp)
endif()

kde4_add_kcfg_files(kwin_SRCS settings.kcfgc)

qt4_add_dbus_adaptor( kwin_SRCS org.kde.KWin.xml dbusinterface.h KWin::DBusInterface )
qt4_add_dbus_adaptor( kwin_SRCS org.kde.kwin.Compositing.xml composite.h KWin::Compositor )
qt4_add_dbus_adaptor( kwin_SRCS org.kde.kwin.Effects.xml effects.h KWin::EffectsHandlerImpl )

qt4_add_dbus_interface( kwin_SRCS
  ${KDEBASE_WORKSPACE_SOURCE_DIR}/ksmserver/org.kde.KSMServerInterface.xml ksmserver_interface)
qt4_add_dbus_interface( kwin_SRCS
    ${KDEBASE_WORKSPACE_SOURCE_DIR}/ksmserver/screenlocker/dbus/org.freedesktop.ScreenSaver.xml screenlocker_interface)

qt4_add_resources( kwin_SRCS resources.qrc )

########### target link libraries ###############

set(kwin_OWN_LIBS
    kdecorations
    kwineffects
)

set(kwin_QT_LIBS
    ${QT_QTDECLARATIVE_LIBRARY}
)

set(kwin_KDE_LIBS
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDECLARATIVE_LIBRARIES}
)

set(kwin_XLIB_LIBS
    ${X11_X11_LIB}
    ${X11_Xext_LIB}
    ${X11_ICE_LIB}
    ${X11_SM_LIB}
    ${X11_Xcursor_LIB}
    ${X11_Xrandr_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xrender_LIB}
    ${X11_Xfixes_LIB}
)

set(kwin_XCB_LIBS
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
    ${XCB_XFIXES_LIBRARIES}
    ${XCB_DAMAGE_LIBRARIES}
    ${XCB_COMPOSITE_LIBRARIES}
    ${XCB_SHAPE_LIBRARIES}
    ${XCB_SYNC_LIBRARIES}
    ${XCB_RENDER_LIBRARIES}
    ${XCB_RANDR_LIBRARIES}
    ${XCB_KEYSYMS_LIBRARIES}
)

set(kwin_OPENGL_LIBS )

find_library(XF86VM_LIBRARY Xxf86vm)
if (XF86VM_LIBRARY)
    set(kwin_XLIB_LIBS ${kwin_XLIB_LIBS} ${XF86VM_LIBRARY})
else()
    add_definitions(-DKWIN_NO_XF86VM)
endif()

if(OPENGL_EGL_FOUND)
    set(kwin_OPENGL_LIBS ${kwin_OPENGL_LIBS} ${OPENGLES_EGL_LIBRARY})
endif()

set(kwinLibs
    ${kwin_OWN_LIBS}
    ${kwin_QT_LIBS}
    ${kwin_KDE_LIBS}
    ${kwin_XLIB_LIBS}
    ${kwin_XCB_LIBS}
    ${kwin_OPENGL_LIBS}
)

kde4_add_executable( kwin ${kwin_SRCS})

target_link_libraries(kwin ${kwinLibs})

if(OPENGL_FOUND)
    if( KWIN_BUILD_OPENGL_1_COMPOSITING )
        set_target_properties(kwin PROPERTIES COMPILE_FLAGS -DKWIN_HAVE_OPENGL_1)
    endif()
  add_subdirectory(opengltest)
  target_link_libraries(kwin kwinglutils ${OPENGL_gl_LIBRARY})
  # -ldl used by OpenGL code
  find_library(DL_LIBRARY dl)
  if (DL_LIBRARY)
    target_link_libraries(kwin ${DL_LIBRARY})
  endif()
elseif(OPENGLES_FOUND)
  target_link_libraries(kwin ${kwinLibs} kwinglesutils ${OPENGLES_LIBRARIES})
  set_target_properties(kwin PROPERTIES COMPILE_FLAGS "-DKWIN_HAVE_OPENGLES")
endif()

install(TARGETS kwin         ${INSTALL_TARGETS_DEFAULT_ARGS} )

if(OPENGLES_FOUND)
  kde4_add_executable( kwin_gles ${kwin_SRCS})
  target_link_libraries(kwin_gles ${kwinLibs} kwinglesutils ${OPENGLES_LIBRARIES})
  set_target_properties(kwin_gles PROPERTIES COMPILE_FLAGS "-DKWIN_HAVE_OPENGLES")
  install(TARGETS kwin_gles         ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif()

########### install files ###############

install( FILES kwin.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} RENAME kwin.kcfg )
install( FILES kwin.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kwin RENAME kwin.notifyrc)
install( FILES org.kde.KWin.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR} )

add_subdirectory(tests)
