project(KDEBASE_WORKSPACE)
# set_package_properties appeared in cmake 2.8.6
# TODO: Remove when kdelibs >= 4.10 will be required
cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)

include(CTest)
include(CTestConfig.cmake)
include(CheckIncludeFiles)
include(CheckFunctionExists)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH} )

#search packages used by KDE
set(QT_MIN_VERSION "4.8.2")

find_package(KDE4 4.14.3 REQUIRED)
include(KDE4Defaults)

option(WITH_XINERAMA "Xinerama support for multi-headed X displays" ON)

find_package(Strigi)

set_package_properties(Strigi PROPERTIES DESCRIPTION "Desktop indexing and search support"
                       URL "http://strigi.sourceforge.net"
                       TYPE REQUIRED
                      )

find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES DESCRIPTION "Support for gzip compressed files and data streams"
                       URL "http://www.zlib.net"
                       TYPE REQUIRED
                      )

find_package(DBusMenuQt 0.6.0)
set_package_properties(DBusMenuQt PROPERTIES DESCRIPTION "Support for notification area menus via the DBusMenu protocol"
                       URL "https://launchpad.net/libdbusmenu-qt"
                       TYPE REQUIRED
                      )

find_package(QImageBlitz)
set_package_properties(QImageBlitz PROPERTIES DESCRIPTION "An image effects library"
                       URL "http://sourceforge.net/projects/qimageblitz"
                       TYPE REQUIRED
                      )

find_package(KDeclarative QUIET CONFIG)
set_package_properties(KDeclarative PROPERTIES DESCRIPTION "KDE Declarative (QML) support from kdelibs"
                       URL "http://www.kde.org"
                       TYPE REQUIRED
                       PURPOSE "Required for building ksmserver and corebindingsplugin (org.kde.plasma.core)"
                      )

macro_optional_find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES DESCRIPTION "The OpenGL libraries"
                       URL "http://www.opengl.org"
                       TYPE OPTIONAL
                      )
macro_optional_find_package(OpenGLES)
set_package_properties(OpenGLES PROPERTIES DESCRIPTION "The OpenGLES libraries"
                       URL "http://www.khronos.org/opengles"
                       TYPE OPTIONAL
                      )
macro_optional_find_package(UDev)
set_package_properties(UDev PROPERTIES DESCRIPTION "The UDev Libraries"
                       TYPE OPTIONAL
                       PURPOSE "Allows support for UPower backend in PowerDevil - STRONGLY RECOMMENDED"
                      )

if(Q_WS_X11)
  find_package(XCB REQUIRED)
  set_package_properties(XCB PROPERTIES DESCRIPTION "X protocol C-language Binding"
                         URL "http://xcb.freedesktop.org"
                         TYPE REQUIRED
                        )

  find_package(X11_XCB)
  set_package_properties(X11_XCB PROPERTIES DESCRIPTION "XCB X11 protocol client library"
                         TYPE REQUIRED
                        )

  find_package(X11 REQUIRED)
  set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                         URL "http://www.x.org"
                         TYPE REQUIRED
                        )

  add_feature_info("Automated testing of X clients" X11_XTest_FOUND
                   "The X11 Testing Resource extension library is useful for automated testing of X clients")
  add_feature_info("libXau" X11_Xau_FOUND "The X11 Authorization Protocol library may be used by KDM")
  add_feature_info("LibXdmcp" X11_Xdmcp_FOUND "The X Display Manager Control Protocol library may be used by KDM")
  if(NOT X11_Xkbfile_FOUND)
    message(FATAL_ERROR "The X11 keyboard layout library was not found. Required for building keyboard modules.")
  endif()
  if(NOT X11_Xcomposite_FOUND)
    message(FATAL_ERROR "The X11 composite library was not found. Required for building the plasma tray and notifications widgets.")
  endif()
  if(NOT X11_Xdamage_FOUND)
    message(FATAL_ERROR "The X11 damaged region extension library was not found. Required for compositing support in KWin.")
  endif()
  if(NOT X11_Xrender_FOUND)
    message(FATAL_ERROR " The X Rendering Extension client library was not found. Required for XRender Compositing backend in KWin.")
  endif()
  if(NOT X11_Xfixes_FOUND)
    message(FATAL_ERROR "The X11 miscellaneous 'fixes' extension library was not found. Required for XRender Compositing backend in KWin.")
  endif()
  if(NOT X11_Xrandr_FOUND)
    message(FATAL_ERROR "The X11 RandR extension library was not found. Required for Multi Screen Support.")
  endif()
  if(NOT OPENGL_FOUND AND NOT OPENGLES_FOUND)
    message(FATAL_ERROR "Either OpenGL or OpenGL ES 2.0 are required for Compositing support in KWin.")
  endif()
  if(NOT X11_Xcursor_FOUND)
    message(FATAL_ERROR "The X11 cursor management library was not found. Required for desktop effects support in KWin.")
  endif()
endif(Q_WS_X11)

macro_optional_find_package(GLIB2 2.0)
set_package_properties(GLIB2 PROPERTIES DESCRIPTION "Low-level core library for data structure handling, portability wrappers, etc."
                       URL "http://www.gtk.org"
                       TYPE OPTIONAL
                       PURPOSE "Needed to build the kxkb keyboard map control program and provide XMMS support in the Now Playing Plasma data engine"
                      )

macro_optional_find_package(Fontconfig)
set_package_properties(Fontconfig PROPERTIES DESCRIPTION "Font access configuration library"
                       URL "http://www.freedesktop.org/wiki/Software/fontconfig"
                       TYPE OPTIONAL
                       PURPOSE "Needed to build font configuration and installation tools"
                      )

#### Python support ( plasma scriptengines ) ####
macro_optional_find_package(PythonLibrary)
set_package_properties(PythonLibrary PROPERTIES DESCRIPTION "Python scripting language"
                       URL "http://python.org"
                       TYPE OPTIONAL
                       PURPOSE "Needed to build plasma scriptengine for python."
                      )

macro_optional_find_package(QJSON)
set_package_properties(QJSON PROPERTIES DESCRIPTION "Library to manage JSON objects with Qt"
                       URL "http://qjson.sourceforge.net/"
                       TYPE OPTIONAL
                       PURPOSE "Required to build Chrome/Chromium support for Plasma Bookmarks Runners"
                      )

find_package(LibGcrypt 1.5.0 REQUIRED QUIET)
set_package_properties(LibGcrypt PROPERTIES
                       TYPE REQUIRED
                       PURPOSE "kwalletd needs libgcrypt to perform PBKDF2-SHA512 hashing"
                      )

macro_optional_find_package(SLP)
set_package_properties(SLP PROPERTIES DESCRIPTION "SLP (Service Location Protocol) implementation"
                       URL "http://www.openslp.org/"
                       TYPE OPTIONAL
                       PURPOSE "Provides SLP support in the network:/ kioslave."
                      )

check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)

include(ConfigureChecks.cmake)
configure_file (config-runtime.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-runtime.h )
configure_file(config-unix.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-unix.h )
configure_file(config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h )
if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
  set(EXPORT_XCURSOR_PATH "XCURSOR_PATH=${CMAKE_INSTALL_PREFIX}/share/icons:$XCURSOR_PATH\":~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons\"; export XCURSOR_PATH")
endif(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
configure_file(startkde.cmake ${CMAKE_CURRENT_BINARY_DIR}/startkde  @ONLY)
configure_file(config-workspace.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-workspace.h )

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/libs)

# libs
add_subdirectory(cmake)
add_subdirectory(libs)


macro_optional_add_subdirectory( systemsettings )

# core apps
macro_optional_add_subdirectory( kcheckpass )
macro_optional_add_subdirectory( kwin )
macro_optional_add_subdirectory( ksmserver )

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
macro_optional_add_subdirectory( ksplash )

macro_optional_add_subdirectory( powerdevil )
macro_optional_add_subdirectory( qguiplatformplugin_kde )
macro_optional_add_subdirectory( ksysguard )

macro_optional_add_subdirectory(kcontrol)
macro_optional_add_subdirectory(klipper)
macro_optional_add_subdirectory(kmenuedit)
macro_optional_add_subdirectory(krunner)
macro_optional_add_subdirectory(solid-actions-kcm)
macro_optional_add_subdirectory(kstartupconfig)
macro_optional_add_subdirectory(freespacenotifier)
macro_optional_add_subdirectory(kscreensaver)
macro_optional_add_subdirectory(kinfocenter)

if (Q_WS_X11)
    macro_optional_add_subdirectory(ktouchpadenabler)
endif (Q_WS_X11)

macro_optional_add_subdirectory(kcminit)
macro_optional_add_subdirectory(khotkeys)
macro_optional_add_subdirectory(kwrited)
macro_optional_add_subdirectory(ksystraycmd)
macro_optional_add_subdirectory(appmenu)

if(X11_Xau_FOUND AND X11_Xdmcp_FOUND)
    macro_optional_add_subdirectory( kdm )
else(X11_Xau_FOUND AND X11_Xdmcp_FOUND)
    message(STATUS "Xau lib or Xdmcp lib was missing. kdm will not compile")
endif(X11_Xau_FOUND AND X11_Xdmcp_FOUND)

# data
macro_optional_add_subdirectory(cursors)

macro_optional_add_subdirectory(plasma)
macro_optional_add_subdirectory(statusnotifierwatcher)
macro_optional_add_subdirectory(kstyles)

# imported from kde-runtime and other sub-projects
macro_optional_add_subdirectory(kdesu)
macro_optional_add_subdirectory(phonon)
macro_optional_add_subdirectory(menu)
macro_optional_add_subdirectory(kwalletd)
macro_optional_add_subdirectory(kwalletmanager)
macro_optional_add_subdirectory(knotify)
macro_optional_add_subdirectory(kuiserver)
macro_optional_add_subdirectory(soliduiserver)
macro_optional_add_subdirectory(solidautoeject)
macro_optional_add_subdirectory(solid-device-automounter)
macro_optional_add_subdirectory(solid-networkstatus)
macro_optional_add_subdirectory(kcmshell)
macro_optional_add_subdirectory(kioslave)
macro_optional_add_subdirectory(kurifilter-plugins)
macro_optional_add_subdirectory(kimgio)
macro_optional_add_subdirectory(renamedlgplugins)
if (Q_WS_X11)
  macro_optional_add_subdirectory(kstart)
endif (Q_WS_X11)
macro_optional_add_subdirectory(kquitapp)
# Background processes
macro_optional_add_subdirectory(kpasswdserver)
macro_optional_add_subdirectory(kdontchangethehostname)
macro_optional_add_subdirectory(kglobalaccel)
macro_optional_add_subdirectory(ktimezoned)
# Command-line tools (e.g. for shell scripts)
if ( UNIX )
   macro_optional_add_subdirectory(kdeeject)
endif ( UNIX )
macro_optional_add_subdirectory(kfile)
macro_optional_add_subdirectory(kiconfinder)
macro_optional_add_subdirectory(kioclient)
macro_optional_add_subdirectory(kioexec)
macro_optional_add_subdirectory(ktraderclient)
macro_optional_add_subdirectory(kreadconfig)
macro_optional_add_subdirectory(kmimetypefinder)
# UI Helper applications
macro_optional_add_subdirectory(drkonqi)
macro_optional_add_subdirectory(knetattach)
macro_optional_add_subdirectory(keditfiletype)
# Default settings, content and config
macro_optional_add_subdirectory(l10n)
macro_optional_add_subdirectory(localization)
macro_optional_add_subdirectory(kde-menu)

########### install files ###############
# install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kde4 DESTINATION ${BIN_INSTALL_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startkde DESTINATION ${BIN_INSTALL_DIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# make the libraries installed from kdebase/workspace available to other projects
# by creating and installing a KDE4WorkspaceConfig.cmake file, which will
# be searched and found by kdelibs/cmake/modules/FindKDE4Workspace.cmake. Alex

# now create the KDE4WorkspaceConfig.cmake file, which will be loaded by 
# kdelibs/cmake/modules/FindKDE4Workspace.cmake and which has to contain all information
# about the libraries installed from kdebase/workspace/libs/ anybody would like to have. Alex

# we need the absolute directories where stuff will be installed too
# but since the variables which contain the destinations can be relative
# or absolute paths, we need this macro to make them all absoulte, Alex
macro(MAKE_INSTALL_PATH_ABSOLUTE out in)
   if (IS_ABSOLUTE "${in}")    # IS_ABSOLUTE is new since cmake 2.4.8
      set(${out} "${in}")
   else (IS_ABSOLUTE "${in}")
      set(${out} "\${KDE4WORKSPACE_INSTALL_DIR}/${in}")
   endif (IS_ABSOLUTE "${in}")
endmacro(MAKE_INSTALL_PATH_ABSOLUTE out in)

make_install_path_absolute(KDE4WORKSPACE_LIB_DIR     ${LIB_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_LIBEXEC_DIR ${LIBEXEC_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_INCLUDE_DIR ${INCLUDE_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_BIN_DIR     ${BIN_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_SBIN_DIR    ${SBIN_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_DATA_DIR    ${DATA_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_CONFIG_DIR  ${CONFIG_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_ICON_DIR    ${ICON_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_KCFG_DIR    ${KCFG_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_LOCALE_DIR  ${LOCALE_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_MIME_DIR    ${MIME_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_SOUND_DIR   ${SOUND_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_TEMPLATES_DIR    ${TEMPLATES_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_WALLPAPER_DIR    ${WALLPAPER_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_KCONF_UPDATE_DIR    ${KCONF_UPDATE_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_AUTOSTART_DIR    ${AUTOSTART_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_XDG_APPS_DIR        ${XDG_APPS_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_XDG_DIRECTORY_DIR   ${XDG_DIRECTORY_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_SYSCONF_DIR ${SYSCONF_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_MAN_DIR     ${MAN_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_INFO_DIR    ${INFO_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_DBUS_INTERFACES_DIR ${DBUS_INTERFACES_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_DBUS_SERVICES_DIR   ${DBUS_SERVICES_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_SERVICES_DIR ${SERVICES_INSTALL_DIR})
make_install_path_absolute(KDE4WORKSPACE_SERVICETYPES_DIR ${SERVICETYPES_INSTALL_DIR})

set(KDE4WORKSPACE_TARGET_PREFIX KDE4Workspace__)

configure_file(KDE4WorkspaceConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/KDE4WorkspaceConfig.cmake" @ONLY)


set(_KDE4WorkspaceConfig_INSTALL_DIR ${LIB_INSTALL_DIR}/KDE4Workspace/cmake)
# places where find_package() looks for FooConfig.cmake files:
# CMake >= 2.6.0 looks in lib/Foo*/cmake/, CMake >= 2.6.3 also looks in
# lib/cmake/Foo*/, which packagers prefer. So they can set the KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR
# option to have kdepimlibs install its Config file there. Alex
if(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
   set(_KDE4WorkspaceConfig_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/KDE4Workspace)
endif(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/KDE4WorkspaceConfig.cmake
        DESTINATION ${_KDE4WorkspaceConfig_INSTALL_DIR})

install(EXPORT kdeworkspaceLibraryTargets 
        NAMESPACE ${KDE4WORKSPACE_TARGET_PREFIX}
        DESTINATION ${_KDE4WorkspaceConfig_INSTALL_DIR}
        FILE KDE4WorkspaceLibraryTargets.cmake )


